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
// sending a few 8 byte angles as fast as possible

//Notizen:
//PWM: analogWrite(in pin, int value), freq = 1000Hz
//      value 0-255 255:100% 
//
//Serial.readBytes(buffer, length)
//
//reads characters from the serial port into a buffer. The function terminates if the determined length has been read, or it times out (see Serial.setTimeout()).
//Parameters
//Serial: serial port object. See the list of available serial ports for each board on the Serial main page.
//buffer: the buffer to store the bytes in. Allowed data types: array of char or byte.
//length: the number of bytes to read. Allowed data types: int.
//maual mode data format: PC sends angles for the servos in degree 
//because 1 byte is 1 number the PC can send 0 to 254 degree. 1=0°, 255 = 254° 
//that system might be chnaged later
//
/* Some ASCII values
 *  33  !
 *  50  2
 *  80  P
 *  100 d
 *  126 ~
 * 
 */

/*********************************************************/

/*
#define PWM2_PIN 2
#define PWM3_PIN 3
#define PWM4_PIN 4
#define PWM5_PIN 5
#define PWM6_PIN 6
#define PWM7_PIN 7
#define PWM8_PIN 8
#define PWM9_PIN 9
*/

//#define ACK 101
//#define NACK 102

#define ACK 97
#define NACK 110
#define TIME_OUT_MS 1000

//first set REC_B_NUM to 2 then 1 and then 0 if it prints no extra byte in SerialReceiveAngles
#define NUM_SERVOS 6

long startTime = 0;

const int pwmPins[NUM_SERVOS] = {2,3,4,5,6,7}; //(wanted) error if number of pins is bigger than NUM_SERVOS

//uint8_t anglesDefault[NUM_SERVOS] = {0,0,0,0,0,0,0,0}; //check numbers!
//uint8_t anglesEnd[NUM_SERVOS] = {0,0,0,0,0,0,0,0};
//uint8_t anglesStart[NUM_SERVOS] = {0,0,0,0,0,0,0,0};
//
//uint8_t pwmEnd[NUM_SERVOS] = {0,0,0,0,0,0,0,0};
//uint8_t pwmStart[NUM_SERVOS] = {0,0,0,0,0,0,0,0};

uint8_t anglesEnd[NUM_SERVOS] = {0,0,0,0,0,0};
uint8_t anglesStart[NUM_SERVOS] = {0,0,0,0,0,0};

uint8_t pwmEnd[NUM_SERVOS] = {0,0,0,0,0,0};
uint8_t pwmStart[NUM_SERVOS] = {0,0,0,0,0,0};

uint8_t recErrF = 0; //Receiving Error flag. set to 1 if there is an error while receiving data

//-------------------------------------------------------------------------------
void setup() {
  //pinMode not needed 
 Serial.begin(115200);
 Serial.setTimeout(TIME_OUT_MS); //for debugging

}

//-------------------------------------------------------------------------------

void loop(){
  uint8_t bt[NUM_SERVOS-1];
  uint8_t angles[NUM_SERVOS];
  uint8_t c, pwm;
  c = Serial.readBytes(bt, 1);
  // Serial.println("Status: " + String(c) + ":   " + String(bt[0]) ); 
  if(c != 0){
    angles[0] = bt[0];
    c = Serial.readBytes(bt, NUM_SERVOS-1);
    if(c != NUM_SERVOS-1){
      sendNACK();
    } else {

      for(int i = 1; i < NUM_SERVOS; i++){
        angles[i] = bt[i-1];
      }
      
      //Set all PWM values
      for(int i = 0; i < NUM_SERVOS; i++){
        pwm = ConvAngleToPWM(angles[i]);
        analogWrite(pwmPins[i], pwm);
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

void SetAllPWM(void){
  for(int i = 0; i < NUM_SERVOS; i++){
    analogWrite(pwmPins[i], pwmEnd[i]);
  }
}




//Later: velocity by sending delay values
