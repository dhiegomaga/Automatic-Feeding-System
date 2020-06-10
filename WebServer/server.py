import sys
sys.path.insert(0,'../SerialCommunicationProtocol')
from SerialProtocolSimple import SerialProtocol
from flask import Flask, render_template, request

app = Flask(__name__)

systen_variables={
    'temp1': 0,
    'temp2': 0,
    'temp3': 0,

    'cooler_set': 0,
    'heater_set': 0,
    'pump_set'  : 0,

    'system_on': 0
}

@app.route('/')
def index():
    global systen_variables

    # Read GPIO Status
    systen_variables['temp1'] = '55 C'

    system_state = 'ON'
    if systen_variables['system_on'] == False:
        system_state = 'OFF'

    templateData = {
        'temp1'  : systen_variables['temp1'],
        'system_state': system_state
    }
    return render_template('index.html', **templateData)

@app.route("/<deviceName>/<action>")
def action(deviceName, action):
    global systen_variables

    if deviceName == 'button1':
        systen_variables['system_on'] = not systen_variables['system_on']
   
    system_state = 'ON'
    systen_variables['temp1'] = '54 C'
    if systen_variables['system_on'] == False:
        system_state = 'OFF'
        systen_variables['temp1'] = '0 C'

    templateData = {
        'temp1'  : systen_variables['temp1'],
        'system_state': system_state
    }
    return render_template('index.html', **templateData)

if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=5000)