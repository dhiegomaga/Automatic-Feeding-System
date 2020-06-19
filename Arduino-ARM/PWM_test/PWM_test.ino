//Initializing LED Pin
#include <Servo.h>

// Declare servos
#define NUM_SERVOS 1
Servo servos[NUM_SERVOS];
const int pwmPins[NUM_SERVOS] = {5};

int led_pin = 5;
int angle = 0;
void setup() {
  //Declaring servo pins
  for(int i=0; i< NUM_SERVOS; i++){
    servos[i].attach(pwmPins[i]);
  }
  Serial.begin(115200);
}
void loop() {
  //analogWrite(led_pin, angle);

  uint8_t bt[1];
  uint8_t c;
  servos[0].write(angle);
  
  c = Serial.readBytes(bt, 1);
  
  if(c == 1){
    angle = (int) (bt[0]-48)*18.0f;
    
    Serial.println(angle);
  }

  delay(15);
}
