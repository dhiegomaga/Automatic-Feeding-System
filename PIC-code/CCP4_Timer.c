/*
 * File:   CCP4_Timer.c
 * Author: denzel
 *
 * Created on 28. Mai 2020, 21:51
 */


#include "CCP4_Timer.h"


void CCP4_TimerSetup(void){
   // setup timer 1
    T1CONbits.TMR1CS = 0; // Timer1 clock source is oscillator clock (Fosc/4). 4MHz
    T1CONbits.T1CKPS = 0b10; // 2^2 -> 1:4 Prescale value -> 1MHz
    T1CONbits.TMR1ON = 1; // Timer on
    T1GCONbits.TMR1GE = 0; // Timer1/ counts regardless of gate function

    // setup capture and compare module CCP1
    CCP4CONbits.CCP4M = 0b1011; // Compare mode with Special Event Trigger, CCP4IF is set
    CCPTMRS1bits.C4TSEL = 0; // timer <-> ccp module (CCP4 / TMR1)
    CCPR4 = 64000; // compare value for setting CCP1IF default 

    // clear overflow flag bits, called IF
   
    PIR1bits.TMR1IF = 0; // clear overflow bit of timer 1
    PIR4bits.CCP4IF = 0; // clear match of compare module CCP1
 
}

//sets timer and resets flags
//max Resolution 1us, 
//imput time in us
//1 increment is 1us -> up to 64000us possible
void CCP4_SetTimer(uint16_t timeSet_par){
    PIR1bits.TMR1IF = 0; // clear overflow bit of timer 1
    PIR4bits.CCP4IF = 0; // clear match of compare module CCP1
    CCPR4 = timeSet_par;
}

//timer counts until match occours
//so a reset timer function might be needed to set the timer back to 0 
//if the match is not needed anymore, or before setting it

//resets timer by letting a match occour after a short amount of time (1us)
void CCP4_Reset(void){
    //set timer with 1us
    CCP4_SetTimer(1);
    while(!PIR4bits.CCP4IF); //wait until match occours -> timer counter is automatically reset
    //reset Flags
    PIR1bits.TMR1IF = 0; // clear overflow bit of timer 1, actually not needed because TMR1 never overflows
    //but to be save...
    PIR4bits.CCP4IF = 0; // clear match of compare module CCP1
}





