#include "PIC_PI_Prot.h"
#include "SerCom.h"
#include "PWM2x.h"
#include "CCP4_Timer.h"

// Expects to read a value between 1-100, with a timeout of 20ms. Returns the value read or 0 if it fails
uint8_t readValue(){
    CCP4_SetTimer(20000);
    while(!(CCP4_F)){
        uint8_t byte = SerialRec();
        if(byte >= 1 && byte <= 100){
            return byte;
        }
    }
    return NACK;
}

void executeCommand(uint8_t command, uint8_t value){
    if(command == CPS){
        // TODO change PWM of pump speed
        if(value > 50){
            LATBbits.LB4 = 0;
        } else {
            LATBbits.LB4 = 1;
        }
        
        DUTY_CYC_1 = (uint8_t) (value/255.0f * 199);
    }
    else if(command == CSS){
        
    } 
    else if(command == CHT){
        
    }
    
    else if(command == CCT){
        
    }
}

//function for the IWT and IDWT part
//returns the serial respons if True, ACK or sth else except NACK
uint8_t SerStartEnd(uint8_t data_par, uint16_t timeWindow_par){
    uint8_t serRec_intern = NACK;
    
    for(int i = 0; i < 3; ++i){
        SerialSend(data_par);
        // timeWindow_par us time window for receiving an answer
        CCP4_SetTimer(timeWindow_par); //10ms
        while(!(CCP4_F)){ //CCP4_F not set
            
            serRec_intern = SerialRec();
            if(serRec_intern != NACK){ //Later do != NACK if == ACK works
                //SerialSend(ACK); //only for testing, if PIC did receive it it will send ACK 
                return serRec_intern; //leave function
            }
        }
        
      }
    
    return serRec_intern; //if nothing received -> returns NACK
    
}

/******************************************************************************
//Test Function SerStartEnd 1:

 void main(void) {
    __init_();
      
    uint8_t serRec = NACK;
    CCP4_SetTimer(50000); //10ms 
    while(42){
        LATBbits.LB2 ^= 1; //LED 1 toggle every loop to see if PIC is running
        
      uint8_t hs = SerStartEnd(IWT, 50000);
     
      if(hs != NACK){
        LATBbits.LB3 ^= 1; //LED2 toggle everytime ACK is received 
      }  
     
    }
    return;
}
 
*******************************************************************************/

/******************************************************************************
//Test Function SerStartEnd 2:

void main(void) {
    __init_();
      
    uint8_t serRec = NACK;
    CCP4_SetTimer(50000); //10ms 
    while(42){
        LATBbits.LB2 ^= 1; //LED 1 toggle every loop to see if PIC is running
        
      uint8_t hs = SerStartEnd(IWT, 50000);
    
      //if ACK received stop main loop (while(42)) until second ACK received
      if(hs == ACK){
         LATBbits.LB3 ^= 1; //LED2 toggle everytime ACK is received
         while(1){
             uint8_t rec = SerialRec();
             
             if(rec == ACK){
                 break;
             }
         }
      }
   
        
    }
    

    return;
}
 
*******************************************************************************/


//Change Pump speed
//returns NACK if failed, duty cycle % if it worked

uint8_t ChangePumpSpeed(void){
    uint8_t state_intern = NACK;
    //Next Number will be Pumpspeed, 
                //Pumpspeed will be set and sent back
                //Pumpspeed is a value between 50 and 100% duty cycle
                //it can also be less, but the pump will not move under 50%
                //there is a extra 10ms time window only for receiving the pump speed
                //so the whole time window will be max 10ms longer but this is no problem
                
                //The PWM Prescaler is 4, the Period Value is 199 -> 5kHz, Duty Cycle value has to be smaller than 199
                uint8_t max_intern = 199;
                CCP4_SetTimer(10000); //reset timer counter, flags and set 10ms
                uint8_t cpsRec_intern = NACK; //NACK > 100!
                while(!(CCP4_F)){ //Time window will only be run throug once -> dont reset flags inside of while
                    cpsRec_intern = SerialRec(); 
                    
                    if(cpsRec_intern){ //cpsRec < 20 turns the pump off
                        Nop();
                        if((cpsRec_intern > 0) && (cpsRec_intern < 20)){
                          DUTY_CYC_1 = 0; //turn pump off  
                          SerialSend(cpsRec_intern);
                          return 0; //break out of while(!(CCP4_F))
                        }else if((cpsRec_intern >= 20) && (cpsRec_intern <= 100)){
                            
                        
                        uint8_t cyc_intern = (uint8_t) ((uint16_t) ((cpsRec_intern * max_intern)/100));
                        DUTY_CYC_1 = cyc_intern; //calculation is done as uint16_t and then converted to 8_t, 
                        //the +-1 of the calculation doesnt matter, if it does and should be +-0.1 -> val*1990/1000
                        if(cyc_intern == max_intern){
                            SerialSend(100);
                            return 100;
                        }
                        uint8_t dutyCycPer_intern = (uint8_t) ((uint16_t) ((cyc_intern * 100)/max_intern + 1));
                        SerialSend(dutyCycPer_intern); //+1 because during back conversion the value is always -1 the earlier value
                        //except for 100
                        return dutyCycPer_intern; //break out of while(!(CCP4_F))
                        }else if((cpsRec_intern > 100) && (cpsRec_intern != NACK)){
                            LATBbits.LB4 ^= 1; //Toggle                          LED3
                           SerialSend(NACK);
                           return NACK; //break out of while(!(CCP4_F))
                        }
                        
                    }
                }//Time Window inside of ==CPS
                //if nothing will arrive in time window, the PIC will send nothing
                //so the RPI knows that the pic didnt receive anything
     return state_intern;           
}


