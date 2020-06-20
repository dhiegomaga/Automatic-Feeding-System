import time
import serial
import threading
pbytes = {
    'ACK': 97,
    'NACK': 110
}

MIN_ANGLE = 10
MAX_ANGLE = 240

class ArmController:

    def __init__(self, starting_angles, port,brate):

        try:
            # Open port
            self.ser = serial.Serial(port, brate, timeout=0.5) # timeout in seconds
            self.ser.close()
            self.ser.open()
            time.sleep(0.1)

            # Define starting angles
            self.num_servos = len(starting_angles)
            self.set_angles     = starting_angles
            self.current_angles = starting_angles

            # Define angle writing speed
            self.delta_angle = 2
            self.delta_time  = 0.02

            # Start talker thread
            self.main_loop = threading.Thread(target=ArmController.talk_loop_thread, args = (self,) )
            self.main_loop.daemon = True

        except Exception as e:
            
            # Terminate program
            raise e
    
    # Connects to ARM and then starts the main thread
    def start(self):
        global pbytes

        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

        time.sleep(0.3)
        
        # Receive NACK once
        self.ser.write(bytes([2,3,5]))
        recv_data = self.ser.read(1)
        while len(recv_data) == 0:
            recv_data = self.ser.read(1)
            print("Waiting for NACK")

        print("Response: ", recv_data[0])

        # Wait for response
        while True:
            self.write_angles(self.set_angles)
            recv_data = self.ser.read(1)
            if len(recv_data) !=0:
                if recv_data[0] == pbytes['ACK']:
                    break

                else: 
                    print("Response different from ACK")
            else: 
                print("ARM not responding, trying again...")

            time.sleep(self.delta_time)

        self.running = True
        self.main_loop.start() # Starts talking thread

    def talk_loop_thread(self):
        global pbytes, MIN_ANGLE, MAX_ANGLE

        # Loops while self.running is active
        while self.running:

            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            time.sleep(self.delta_time)


            # Calculate next angles to set
            next_angles= [0]*self.num_servos
            for i in range(len(self.set_angles)):
                curr_a = self.current_angles[i]
                set_a = self.set_angles[i]
                if set_a > curr_a:
                    next_angles[i] = curr_a + self.delta_angle
                    next_angles[i] = self.clamp(next_angles[i], MIN_ANGLE, set_a)
                
                elif set_a < curr_a:
                    next_angles[i] = curr_a - self.delta_angle
                    next_angles[i] = self.clamp(next_angles[i], set_a, MAX_ANGLE)

                else:
                    next_angles[i] = set_a

            while True:
                self.write_angles(next_angles)
                recv_data = self.ser.read(1)
                if len(recv_data) !=0:
                    if recv_data[0] == pbytes['ACK']:
                        self.current_angles = next_angles
                        break

                    else: 
                        print("Response: ", recv_data[0])
                else: 
                    print("ARM not responding, trying again...")

            

    def setAngles(self, angles):
        if len(angles) == self.num_servos:
            self.set_angles = angles

        else:
            print("Number of angles set differ from number of servos")

    # Terminate serial communication
    def close(self):
        print("Closing ARM connection")

        self.running = False
        self.ser.close()

    def write_angles(self, angles):
        global MIN_ANGLE, MAX_ANGLE
        angles = [self.clamp(x*1.41667, MIN_ANGLE, MAX_ANGLE) for x in angles]
        # print("Writing angles: ", angles)
        byte_angles = bytes(angles)
        self.ser.write(byte_angles)

    def clamp(self, n, minn, maxn):
        return int(max(min(maxn, n), minn))