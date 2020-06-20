import sys
import os
import time
sys.path.insert(0, os.path.join('..', 'SerialCommunicationProtocol'))
sys.path.insert(0, os.path.join('..', 'Arduino-ARM'))
from SerialProtocolSimple import SerialProtocol
from ArmController import ArmController

from flask import Flask, render_template, request,jsonify

app = Flask(__name__)

system_variables={
    'temp0'     : 0,
    'temp1'     : 0,
    'temp2'     : 0,
    'cooler_set': 0,
    'heater_set': 0,
    'pump_set'  : 0,
    'system_on' : False,
    'Birds'     : 0
}

MIN_TEMP = -19
MAX_TEMP = 80
MIN_PUMP_SPEED = 0
MAX_PUMP_SPEED = 100

# PIC
protocol = None
pic_port = '/dev/ttyACM0'
pic_brate = 9600
pic_verbose = False

# Arm Controller
arm_controller = None
arm_port = '/dev/ttyACM1'
arm_brate = 115200
robot_home_position = [90,165,50,90,0]
robot_bird_position = [30,90,100,90,0]

def initialize():
    global protocol, pic_port, pic_brate, pic_verbose
    global arm_controller, arm_port, arm_brate, robot_home_position, robot_bird_position
    
    try: 
        protocol = SerialProtocol(pic_port, pic_brate, pic_verbose)
        protocol.start()

        arm_controller = ArmController(robot_home_position, arm_port, arm_brate)
        arm_controller.start()

        app.run(debug=True, host="localhost", port=5000, use_reloader=False)

    except KeyboardInterrupt:
        print("Keyboard interrupt")

    except Exception as e:
        print(e)

    if protocol:
        protocol.close() # Close protocol

    if arm_controller:
        arm_controller.close()

    print("Terminating")
    exit(0)

@app.route('/')
def index():
    global system_variables
    update()

    system_state = 'ON'
    if system_variables['system_on'] == False:
        system_state = 'OFF'

    templateData = {
        'temp0'  : system_variables['temp0'],
        'temp1'  : system_variables['temp1'],
        'temp2'  : system_variables['temp2'],
        'Birds'  : system_variables['Birds'],
        'pump_set': system_variables['pump_set'],
        'cooler_set': system_variables['cooler_set'],
        'heater_set':system_variables['heater_set'],
        'system_state': system_state
    }
    return render_template('index.html', **templateData)

@app.route("/setPumpSpeed", methods = ['POST'])
def setPumpSpeed():
    global system_variables, MIN_PUMP_SPEED, MAX_PUMP_SPEED
    update()

    system_variables['pump_set'] = clamp(int(request.json['value']), MIN_PUMP_SPEED, MAX_PUMP_SPEED)

    actual_speed = system_variables['pump_set']+1
    if actual_speed > 100:
        actual_speed = 100
    protocol.put_command('CPS', actual_speed)

    return jsonify({
        'result':'ok'
    })

@app.route("/state")
def state_update():
    global system_variables
    update()

    system_variables['system_on'] = not system_variables['system_on']
    return jsonify({
        'result':'ok'
    })


@app.route("/robot_go")
def Robot_go():
    global system_variables, arm_controller, robot_bird_position
    update()

    if arm_controller:
        arm_controller.setAngles(robot_bird_position)

    return jsonify({
        'result':'ok'
    })

@app.route("/robot_back")
def Robot_back():
    global system_variables, arm_controller, robot_home_position
    update()

    if arm_controller:
        arm_controller.setAngles(robot_home_position)
    
    return jsonify({
        'result':'ok'
    })

@app.route("/setCoolerTemp", methods = ['POST'])
def set_cooler_temp():
    global system_variables, MIN_TEMP, MAX_TEMP, protocol
    update()

    system_variables['cooler_set'] = clamp(int(request.json['value']), MIN_TEMP, MAX_TEMP)

    actual_temp = system_variables['cooler_set']+20
    protocol.put_command('CCT', actual_temp)

    return jsonify({
        'result':'ok'
    })

@app.route("/setHeaterTemp", methods = ['POST'])
def set_Heater_temp():
    global system_variables, MIN_TEMP, MAX_TEMP
    update()

    system_variables['heater_set'] = clamp(int(request.json['value']), MIN_TEMP, MAX_TEMP)

    actual_temp = system_variables['heater_set']+20
    protocol.put_command('CHT', actual_temp)

    return jsonify({
        'result':'ok'
    })

@app.route('/update', methods = ['GET'])
def updateRoute():
    update()
    return jsonify({
        'result':'ok'
    })

def clamp(n, minn, maxn):
    return max(min(maxn, n), minn)

def update():
    global protocol, system_variables

    protocol.put_command('IWR', 1)
    time.sleep(0.2)
    system_variables['temp0'] = protocol.temp0[0] -20
    system_variables['temp1'] = protocol.temp1[0] -20
    system_variables['temp2'] = protocol.temp2[0] -20

if __name__ == "__main__":
    initialize()
    
