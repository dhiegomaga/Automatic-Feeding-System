/*
 * File:   PIC_PWM3x.c
 * Author: Megaport
 *
 * Created on 26. Mai 2020, 22:03
 */



#include "PWM2x.h"

void SetupAllPWM(void){
    //Multiple PWM signals with only one timer on the same Frequency
     //but different dutycycles
     //several PWM with the same Timer unexpectedly works
     //why?
     /*
      * the PR value corresponds only to the freq. but the freq can be the same for all pwm 
      * Signals.
      * only the duty cycle has to be changen and that doesnt change anything for the timer!
      * 
      * 
      * PWM Period = (PRx + 1)*4*TOSC*(TMRx Prescale Value)
      * TOSC = 1/FOSC
      * FOSC = 16MHz -> TOSC = 0,0625us
      * 
      * duty cycle Resolution = log(4*(PRx+1))/log(2) bits
      * 
      * Prescaler   PR2     PWM Period      PWM Freq        Resolution
      * 00 1        0       0,25us          4000kHz         2bits
      *             255     64us            15,6kHz         10bits (8bits)
      * 01 4        0       1us             1000kHz         2bits
      *             255     256us           3,9kHz          10bits (8bits)
      * 1x 16       0       4us             250kHz          2bits
      *             255     1024us          0,977kHz        10bits (8bits)
      * 
      * set Prescaler: 1x 4
      * Period/1us - 1 = PR2
      * (1/(1us * F_PWM)) - 1 = PR2
      * F_PWM = 10kHz -> PR2 = 99 exakt
      * Resolution = 8,6 bits -> 8 bits: 255 steps -> 1,5625% duty cycle resolution
      * 
      * working LED4 1khz:
      * PR2 = 240
      * prescaler: 10 16
      * 
      */
    //RB5 CCP3 and LED4
     ANSELBbits.ANSB5 = 0;
     //ANSELCbits. = 0; //ANSC1 doesnt exist. maybe pin reserved for sth else
     ANSELCbits.ANSC2 = 0;
     TRISCbits.RC2 = 1;          //1. Disable output driver on CCP1 pin = RC2
     //TRISCbits.RC1 = 1;         //Disable output driver on CCP2 pin = RC1
     TRISBbits.RB5 = 1;         //CCP3
     CCPTMRS0bits.C1TSEL = 0b00; //2. select Timer2 for CCP1
     CCPTMRS0bits.C2TSEL = 0b00; //2. select Timer2 for CCP2
     CCPTMRS0bits.C3TSEL = 0b00; //CCP3
     
     CCP1CONbits.CCP1M = 0b1100; //4. CCP1 0b11xx PWM Mode
     CCP2CONbits.CCP2M = 0b1100; //4. CCP2 0b11xx PWM Mode
     CCP3CONbits.CCP3M = 0b1100;
     PR2 =  199; //199: 5kHz     //3. slect PR value for Timer2 0-255(IR set after TMR2 = PR2), for Changing Freq. this Value equals 100% duty cycle
     DUTY_CYC_1 = 50;               //5. CCP1 select duty cycle 0-255, can be changed anytime
     DUTY_CYC_2 = 0;              //5. CCP2 select duty cycle 0-255, can be changed anytime, 
     DUTY_CYC_3 = 80;              //RB5, LED4
     
     //6. config and enable Timer2
     PIR1bits.TMR2IF = 0;       //clear IR Flag
     T2CONbits.T2CKPS = 0b01;   //Prescaler 00 1:1, 01 1:4, 1x 1:16, for changing Freq
     T2CONbits.TMR2ON = 1;      //Timer2 on

     while(!PIR1bits.TMR2IF); //wait until timer overflows
     TRISCbits.RC2 = 0; //enable output driver on RC2
     TRISCbits.RC1 = 0; //enable output driver on RC1
     TRISBbits.RB5 = 0;         //CCP3
    
}


