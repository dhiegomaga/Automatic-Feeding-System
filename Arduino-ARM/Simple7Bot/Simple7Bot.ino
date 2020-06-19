/*******************************************************/
//Very Simple Robot Control, to have sth that works
//Due PWM Pins: 2...9
//manuell control mode: receives angles via usb and moves with full speed, -> send angles in small steps
//
//The DUE receives angles Via Serial, sets the coresponding PWM Values and sends back the last and new values (can be disabled)
//So to move the arm in a smooth way the PC has to send values with only 1° difference every few ms depending on how fast the arm should move

//Send always all algles if not all angles are sent the DUE will wait until timeout for the rest of the angles
//will only change the pwm if at least 1 angle of the sent angles is different from the last ones

//executing Time in debugging Mode (sending all algles back) : ~40ms
//without sending angles Back: 3ms (its 3ms everytime)

//execution time might seem longer because it waits for the user input
//To test the Execution Time time sth like:
// 123456788765432112345678876543211234567887654321
// max output: ~ = 126 = 1,15V (measuring with Multimeter set on DC)
// ~1~~~~~~~~~~~~1~
// min output: ! = 33 = 0,3V
// !~!~!~!~~!~!~!~!
//#define ACK 101
//#define NACK 102

#include <Servo.h>

#define ACK 97
#define NACK 110
#define TIME_OUT_MS 100
#define NUM_SERVOS 5

Servo servos[NUM_SERVOS];
const int pwmPins[NUM_SERVOS] = {2,3,4,5,6}; //(wanted) error if number of pins is bigger than NUM_SERVOS
int angles[NUM_SERVOS] = {90,165,50,90,0}; // Initial angles

//-------------------------------------------------------------------------------
void setup() {
  //pinMode not needed 
 Serial.begin(115200);
 Serial.setTimeout(TIME_OUT_MS); //for debugging
 for(int i=0; i< NUM_SERVOS; i++){
    servos[i].attach(pwmPins[i]);
    servos[i].write(angles[i]);
  }

  Serial.flush();
  delay(500);
}

//-------------------------------------------------------------------------------

void loop(){
  uint8_t bt[NUM_SERVOS-1];
  uint8_t c;
  c = Serial.readBytes(bt, 1);
  // Serial.println("Status: " + String(c) + ":   " + String(bt[0]) ); 
  
  if(c != 0){
    angles[0] = byteToAngle(bt[0]);
    c = Serial.readBytes(bt, NUM_SERVOS-1);
    if(c != NUM_SERVOS-1){
      sendNACK();
    } else {

      for(int i = 1; i < NUM_SERVOS; i++){
        angles[i] = byteToAngle(bt[i-1]);
      }

      //Set all PWM values
      for(int i = 0; i < NUM_SERVOS; i++){
        servos[i].write(angles[i]);
        //Serial.println("Servo "+ String(i)+": "+String(angles[i]) );
      }
      
      sendACK();
    }
  }

}

void sendACK(){
  Serial.write(ACK);
}

void sendNACK(){
  Serial.write(NACK);
}

// Converts 0-255 to 0-180
int byteToAngle(uint8_t bt){
  return (int) bt * 0.705882f;
}

//-----------------------------------------------------------
  //To Do calculate angle resolution
  //The RPI can go up to 12 bit resolution, the calculation would need to be ajusted
  //currently the resolution is around +-1°
  //if the possible angle range is decreased the resolution could be increased, but then it might be easier to calculate
  //the PWM value on the RPI and only use the DUE to set the PWM Values
  
//Input: 
// angle_par - value between 1 and 255 coresponding to angle between 0 an 254
// lastPWM_par - the last PWM value
// instead of the angle the RPI could also send the PWM if it would be easier to code it in Phython
//-----------------------------------------------------------

uint8_t ConvAngleToPWM(uint8_t angle_par){
  uint8_t pwm_intern = 0; //0-255

  pwm_intern = (uint8_t) angle_par * 255.0f / 360.0f; //Calculate PWM Value (+-1.4° while conversion to int)

  return pwm_intern;
}





//Later: velocity by sending delay values
