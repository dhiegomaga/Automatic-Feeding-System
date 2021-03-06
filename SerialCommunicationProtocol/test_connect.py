from SerialProtocolSimple import SerialProtocol
import time

port = 'COM3'
port = '/dev/ttyACM0'

brate = 9600
verbose = False
protocol = None

try:
    protocol = SerialProtocol(port, brate, verbose)
    protocol.start()

    while True:

        in_str = input("Type a command: ")
        strs = in_str.split(' ')

        try:
            if len(strs) == 2: # Set command 
                command = strs[0]
                speed = int(strs[1])
                protocol.put_command(command, speed)

            else:
                print("Wrong command format")

            time.sleep(0.1) # Sleep to let text of interaction be written

            # Test if thread is still running
            if not protocol.isAlive():
                raise Exception("FATAL ERROR, TERMINATING")

        except RuntimeError as e:
            print(e)

        except ValueError as e:
            print("Invalid command value")

except KeyboardInterrupt:
    # User kyeboard interrupt
    print("Keyboard interrupt. ")

except Exception as e: 
    # Unhandled error
    print(e) 

finally: 
    if protocol:
        protocol.close() # Close port
    print("Exiting")