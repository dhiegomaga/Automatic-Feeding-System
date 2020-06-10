import sys
sys.path.insert(0,'../SerialCommunicationProtocol')
from SerialProtocolSimple import SerialProtocol
from flask import Flask, render_template, request,jsonify

app = Flask(__name__)

system_variables={
    'temp0': 0,
    'temp1': 0,
    'temp2': 0,

    'cooler_set': 0,
    'heater_set': 0,
    'pump_set'  : 0,

    'system_on': 0
}

@app.route('/')
def index():
    global system_variables

    # Read GPIO Status
    system_variables['temp1'] = '55 C'

    system_state = 'ON'
    if system_variables['system_on'] == False:
        system_state = 'OFF'

    templateData = {
        'temp0'  : system_variables['temp0'],
        'temp1'  : system_variables['temp1'],
        'temp2'  : system_variables['temp2'],

        'pump_set': system_variables['pump_set'],
        'system_state': system_state
    }
    return render_template('index.html', **templateData)

@app.route("/debug")
def debug_vlues():
    global system_variables
    system_variables['temp0'] = '85 C'
    return jsonify({
        'result':'ok'
    })

@app.route("/setPumpSpeed", methods = ['POST'])
def setPumpSpeed():
    global system_variables

    system_variables['pump_set'] = request.json['value']

    return jsonify({
        'result':'ok'
    })


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=5000)