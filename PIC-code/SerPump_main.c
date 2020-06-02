/*
 * File:   SerPump_main.c
 * Author: denzel
 *
 * Created on 28. Mai 2020, 22:05
 * 
 * simplified version
 * 
 * functions:
 * void CCP4_TimerSetup(void);
void CCP4_SetTimer(uint16_t);
void CCP4_Reset(void); //use only if its sure that all flags are set and need to be reset
//if not and it is called in a loop before set timer -> the code might not work

 * void SetupAllPWM(void);
 * 
 * void SetupSerial(void);

void DataFun(uint8_t); //does sth with the recData

void SerialSend(uint8_t data_par);
uint8_t SerialRec(void);
 * 
 */


#include "CCP4_Timer.h"
#include "PWM2x.h"
#include "SerCom.h"
#include "PIC_PI_Prot.h"

void __init_(void);


void main(void) {
    __init_();
      
    uint8_t serRec = NACK;
    //CCP4_SetTimer(50000); //10ms 
    while(42){
        LATBbits.LB2 ^= 1; //LED 1 toggle every loop to see if PIC is running   LED1
        
        uint8_t hs = SerStartEnd(IWT, 50000);
      
        CCP4_SetTimer(10000); //10ms
        for(int i = 0; i < 3750; i++){
            CCP4_SetTimer(10000); //reset flags and set 10ms
            while(!(CCP4_F)){ //10ms time window inside for(75)

                serRec = SerialRec();
                uint8_t value = 0;
                if(serRec == CPS || serRec == CSS || serRec == CHT || serRec == CCT){
                    value = readValue();
                    if(value == NACK){
                        SerialSend(NACK);
                        continue;
                    }
                    
                    executeCommand(serRec, value);
                    SerialSend(ACK);
                    // LATBbits.LB3 ^= 1; //toggle LED 2             
            
                } else if (serRec == IWR){
                    // Send all temperatures 
                    SerialSend(48);
                    SerialSend(50);
                }
        
            } //10ms time window inside for 750ms
          
        } //for 750ms
        hs = SerStartEnd(IDWT, 50000);
        __delay_ms(1000); // IDWT time window
   
        
    } //while 42
    
    
    return;
}


void __init_(void){
    OSCCONbits.IRCF = 0b111; //16MHz
    CCP4_TimerSetup();
    SetupAllPWM();
    SetupSerial();
    RCSTA1bits.SPEN = 1; //1: enables EUSART and config Rx/Tx
    ANSELB = 0x00;
    TRISB = 0x00;
    LATB = 255;
    
    
}











