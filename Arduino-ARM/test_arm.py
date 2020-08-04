from ArmController import ArmController

import time

port = 'COM3'
port = '/dev/ttyACM0'

brate = 115200
starting_angles = [90,165,50,90,0]
arm_controller = None

try:
    arm_controller = ArmController(starting_angles, port, brate)
    arm_controller.start()

    while True:
        time.sleep(2)
        arm_controller.setAngles([50,90,100,90,0])
        time.sleep(4)
        arm_controller.setAngles(starting_angles)
        time.sleep(2)
        arm_controller.setAngles([130,90,100,90,0])
        time.sleep(3)
        arm_controller.setAngles(starting_angles)

except KeyboardInterrupt:
    # User kyeboard interrupt
    print("Keyboard interrupt. ")

except Exception as e: 
    # Unhandled error
    print(e) 

finally: 
    if arm_controller:
        arm_controller.close() # Close port
    print("Exiting")