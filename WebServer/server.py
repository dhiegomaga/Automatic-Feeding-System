import sys
import os
sys.path.insert(0, os.path.join('..', 'SerialCommunicationProtocol'))
#from SerialProtocolSimple import SerialProtocol
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
    'Birds'     : 1,
    'Robot_on'  :  False
}

MIN_TEMP = -20
MAX_TEMP = 80
MIN_PUMP_SPEED = 0
MAX_PUMP_SPEED = 100

@app.route('/')
def index():
    global system_variables

    # Read GPIO Status
    system_variables['temp1'] = 55
    system_variables['temp2'] = 85 

    system_state = 'ON'
    if system_variables['system_on'] == False:
        system_state = 'OFF'

    Robot_Arm = 'Go'
    # if system_variables['Robot_on'] == False:
    #     Robot_Arm = 'Back'

    templateData = {
        'temp0'  : system_variables['temp0'],
        'temp1'  : system_variables['temp1'],
        'temp2'  : system_variables['temp2'],
        'Birds'  : system_variables['Birds'],
        'pump_set': system_variables['pump_set'],
        'cooler_set': system_variables['cooler_set'],
        'heater_set':system_variables['heater_set'],
        'system_state': system_state,
        'Robot_Arm'   : Robot_Arm
    }
    return render_template('index.html', **templateData)

@app.route("/debug")
def debug_vlues():
    global system_variables
    system_variables['temp0'] = 85
    return jsonify({
        'result':'ok'
    })



@app.route("/setPumpSpeed", methods = ['POST'])
def setPumpSpeed():
    global system_variables, MIN_PUMP_SPEED, MAX_PUMP_SPEED

    system_variables['pump_set'] = clamp(int(request.json['value']), MIN_PUMP_SPEED, MAX_PUMP_SPEED)

    return jsonify({
        'result':'ok'
    })

@app.route("/state")
def state_update():
    global system_variables
    system_variables['system_on'] = not system_variables['system_on']
    return jsonify({
        'result':'ok'
    })



@app.route("/Robot")
def Robot_update():
    global system_variables
    # system_variables['Robot_on'] = not system_variables['Robot_on']
    return jsonify({
        'result':'ok'
    })



@app.route("/setCoolerTemp", methods = ['POST'])
def set_cooler_temp():
    global system_variables, MIN_TEMP, MAX_TEMP

    system_variables['cooler_set'] = clamp(int(request.json['value']), MIN_TEMP, MAX_TEMP)

    return jsonify({
        'result':'ok'
    })
@app.route("/setHeaterTemp", methods = ['POST'])
def set_Heater_temp():
    global system_variables, MIN_TEMP, MAX_TEMP

    system_variables['heater_set'] = clamp(int(request.json['value']), MIN_TEMP, MAX_TEMP)

    return jsonify({
        'result':'ok'
    })

def clamp(n, minn, maxn):
    return max(min(maxn, n), minn)


if __name__ == "__main__":
    app.run(debug=True, host="localhost", port=5000)
