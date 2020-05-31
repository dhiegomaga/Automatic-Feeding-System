from SerialProtocolSimple import SerialProtocol

port = 'COM4'
port = '/dev/ttyACM0'

brate = 9600
verbose = True

try:
    protocol = SerialProtocol(port, brate, verbose)
    protocol.start()

    while True:
        in_str = input("Type a command: ")
        strs = in_str.split(' ')
        if len(strs) == 2: # Set command
            command = strs[0] 
            if command == 'cps':
                speed = int(strs[1])

                protocol.put_command(command, speed)

except KeyboardInterrupt:
    # User kyeboard interrupt
    print("Keyboard interrupt. ")

except BufferError as e:
    # Max number of commands reached
    print(e) 

except Exception as e: 
    # Unhandled error
    print(e) 

finally: 
    protocol.close() # Close port
    print("Exiting")