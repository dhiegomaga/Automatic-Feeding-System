# Protocol for Pic-RPI Communication

'''
-------BYTES TABLE-------------------

    1-100: control signal

    101: ack 
    - Acknowledge package

    102: nack
    - Error on package

    103: cps
    - Change pump speed (sent by RPI)

    104: cht
    - Change heater temperature (sent by RPI)

    105: cct
    - Change cooler temperature (sent by RPI)

    106: css
    - Change system state [control signal = ON:100|OFF:1] (sent by RPI)

    201: iwt
    - Start communication window (sent by PIC)

    202: idwt
    - End communication window (sent by PIC)

    203: iwr
    - Read current system tempratures (sent by RPI)

-------COMMUNICATION EXAMPLES--------
    PIC (left) - RPI (right)

    --> ACK : PIC Acknowledges package
    <-- ACK : RPI Acknowledges package
    -->x IWT : PIC IWT is lost
    x<-- CPS : RPI CPS is lost

    Example 1: Control pump speed 
    -->     IWT
    x<--    ACK
    -->     IWT
    <--     ACK
    <--     CPS
    <--     52
    -->     ACK
    -->     IDWT
    <--     ACK

    Example 2: Set system state
    <--     CSS
    <--     1 (OFF)
    -->x    ACK
    <--     CSS
    <--     1 (OFF)
    -->     ACK

    Example 3: Read sensors - NO ACK
    <--     IWR
    -->     56 (temp_0)
    -->     23 (temp_1)

    Example 4: Read sensors, RPI receiving error
    <--     IWR
    -->     56 (temp_0)
    -->x    23 (temp_1)
    ...     (RPI waits for second temp, but doesn't get it)
    <--     IWR
    -->     56 (temp_0)
    -->     23 (temp_1)
    
'''

import time
import serial
import queue
import threading

pbytes = {
    'MIN': b'\x01',
    'MAX': b'\x64',
    'ACK': b'\x65',
    'NACK': b'\x66',
    'CPS': b'\x67',
    'CHT': b'\x68',
    'CCT': b'\x69',
    'CSS': b'\x6A',
    'IWT': b'\xC9',
    'IDWT': b'\xCA',
    'IWR': b'\xCB'
}

inverse_pbytes = {v: k for k, v in pbytes.items()}

class SerialProtocol:

    # Initialize variables and serial communication object
    def __init__(self, port, brate, verbose=False):     
        self.verbose    = verbose
        self.running    = False
        self.talking    = False         # Is PIC in IWT state?
        self.ser        = None
        self.commands   = queue.Queue()

        # Hard-coded Parameters 
        self.PACKAGE_DELAY      = 0.005 # Sleep time between sending packages, in seconds
        self.MAX_TRIALS         = 3     # Max communication attempts before giving up 
        self.COMMAND_QUEUE_SIZE = 3     # Max number of commands in queue

        # Other variables
        self.pending_command = None
        self.temp0 = 0
        self.temp1 = 0
        self.temp2 = 0

        try:
            # Open port
            self.ser = serial.Serial(port, brate, timeout=0) # timeout=None: wait forever until byte is read
            self.ser.close()
            self.ser.open()
            time.sleep(0.1)
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()

            # Start talker thread
            self.main_loop = threading.Thread(target=SerialProtocol.talk_loop_thread, args = (self,) )
            self.main_loop.daemon = True

        except Exception as e:
            if self.verbose:
                print("Error starting program")
            
            # Terminate program
            raise e

        if self.verbose:
            print("Opened port=",port, ", brate=",brate )

    # Connects to PIC and then starts the main thread
    def start(self):
        self.running = True
        self.connect()
        self.main_loop.start() # Starts talking thread

    def isAlive(self):
        self.main_loop.join(timeout=0.0)
        return self.main_loop.is_alive()

    # Wait until PIC sends IWT and answers IWR
    def connect(self):
        global pbytes        

        # Send IWR and wait for response
        while True:
            print("Connecting...")

            self.talking = False
            self.ser.flush()

            # Wait until IWT
            while True:
                byte = self.read_byte()
                if byte == pbytes['IWT']:
                    self.send_byte(pbytes['ACK'])
                    break

                elif byte == pbytes['IDWT']:
                    self.send_byte(pbytes['ACK'])

            self.talking = True
            if self.verbose:
                print("Talking resumed")

            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()

            # Read 3 bytes
            self.send_byte(pbytes['IWR'])
            temp0 = self.read_blocking()

            if temp0 == pbytes['IWT']:
                self.send_byte(pbytes['ACK'])
                continue

            temp1 = self.read_blocking()

            if temp1 == pbytes['IWT']:
                self.send_byte(pbytes['ACK'])
                continue

            temp2 = self.read_blocking()

            if temp2 == pbytes['IWT']:
                self.send_byte(pbytes['ACK'])
                continue

            if self.in_range(temp1) and self.in_range(temp2):
                
                print("Connected")
                return True

            if self.verbose:
                print("Invalid response from PIC, trying again.")

            # Sleep time until sending new packages
            time.sleep(self.PACKAGE_DELAY)       

    # Terminate serial communication
    def close(self):
        if self.verbose:
            print("Closing port and terminating")

        self.ser.close()
        self.running = False

    # Main loop, sends packages stored on queue and get response
    def talk_loop_thread(self):
        global pbytes
        
        # Loops while self.running is active
        while self.running:

            if self.talking:
                # SET NEXT COMMAND TO BE EXECUTED
                next_command = None

                # Check pending commands from last unfinished execution
                if self.pending_command:
                    # Handle pending command
                    next_command = self.pending_command

                elif not self.commands.empty():
                    next_command = self.commands.get()

                # EXECUTE NEXT COMMAND
                if next_command:
                    command,value = next_command
                    assert command in [pbytes['IWR'], pbytes['CPS'], pbytes['CHT'], pbytes['CCT'], pbytes['CSS']], "Invalid command in queue"
                    assert self.in_range(value), "Value not in valid range"

                    self.pending_command = (command,value)
                    status = None

                    if command == pbytes['IWR']:
                        # Read command
                        status = self.execute_read_command()

                    else : # Set value command
                        status = self.execute_set_command(command, value)

                    # Check result
                    if status: # Success
                        self.pending_command = None # Clear pending command

                    else: # Failed to execute command
                        if self.verbose:
                            print("Failed to execute command")
                    
                else: # no commands to be executed
                    byte = self.read_byte()
                    if byte == pbytes['IDWT']:
                        self.send_byte(pbytes['ACK'])

                        if self.verbose:
                            print("Talking paused")
                        self.talking = False

            else: # PIC not talking

                # Read 1 byte and test if it is IWT
                byte = self.read_byte()

                if byte == pbytes['IWT']:
                    self.send_byte(pbytes['ACK'])

                    if self.verbose:
                        print("Talking resumed")
                    self.talking = True
                    
            time.sleep(self.PACKAGE_DELAY)

    # Puts a command in queue
    # command: the command in byte form
    # value: optional value
    def put_command(self, command_str, value):
        global pbytes

        if self.commands.qsize() >= self.COMMAND_QUEUE_SIZE:
            raise RuntimeError("Max number of commands in queue reached")
        
        # Convert int to byte
        if type(value) == int:
            value = bytes([value])

        # Convert command from string to byte
        if command_str not in pbytes:
            raise RuntimeError("Command not found: ", command_str)
        command = pbytes[command_str]

        # Check type of value and command
        assert type(command) == bytes, 'put_command() command must be bytes or int'
        assert type(value) == bytes, 'put_command() value must be bytes or int'

        self.commands.put((command, value))
    
    # Reads temperature state
    def getState(self):
        return [self.temp0, self.temp1, self.temp2]

    # Executes read command, returns True if read was successful
    def execute_read_command(self):
        global pbytes

        if self.verbose:
            print("Executing READ command")

        self.send_byte(pbytes['IWR'])

        temp0 = self.read_blocking()
        if not self.in_range(temp0):
            return False

        temp1 = self.read_blocking()
        if not self.in_range(temp1):
            return False

        temp2 = self.read_blocking()
        if not self.in_range(temp2):
            return False

        self.temp0 = temp0
        self.temp1 = temp1
        self.temp2 = temp2

        # print("temp0:",ord(temp0))
        # print("temp1:",ord(temp1))
        # print("temp2:",ord(temp2))

        return True

    # Executes set command, returns true if set was successful
    def execute_set_command(self, command, value):
        global pbytes, inverse_pbytes
            

        # Clears serial buffer
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

        # Send commands
        self.send_byte(command)
        time.sleep(self.PACKAGE_DELAY)
        self.send_byte(value)

        # Waits for ACK
        byte = self.read_blocking()

        if byte == pbytes['ACK']:
            # TODO save state
            print("Completed:",inverse_pbytes[command], ':',value[0])
            return True

        else: 
            return False
    
    # Check if byte is between 1 and 100
    def in_range(self, byte):
        global pbytes
        return byte >= pbytes['MIN'] and byte <=pbytes['MAX']

    # Sends 1 byte
    def send_byte(self, byte):
        global inverse_pbytes

        self.ser.write(byte)
        if self.verbose:
            if byte in inverse_pbytes:
                print("SENT DATA: ", inverse_pbytes[byte])
            else:
                print("SENT DATA: ", ord(byte))

    # Reads 1 byte, returns (byte, status)
    def read_byte(self):
        global inverse_pbytes

        recv_data = self.ser.read(1)
        if self.verbose and recv_data != b'':
            if recv_data in inverse_pbytes:
                print("READ DATA: ", inverse_pbytes[recv_data])
            else:
                print("READ DATA: ", ord(recv_data))

        return recv_data

    def read_blocking(self):
        global inverse_pbytes

        recv_data = self.ser.read(1)
        while recv_data == b'\x00' or recv_data == b'':
            recv_data = self.ser.read(1)

        if self.verbose:
            if recv_data in inverse_pbytes:
                print("READ DATA: ", inverse_pbytes[recv_data])
            else:
                print("READ DATA: ", ord(recv_data))

        return recv_data
