#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H



#pragma config FOSC = INTIO67 //  Internal oscillator block bis 16MHz
#pragma config WDTEN = OFF // Watchdog Timer Enable bits ( Watch dog timer disabled )
#pragma config LVP = OFF // Single - Supply ICSP Enable bit (Single - Supply ICSP disabled )


#include <xc.h>
#include <stdint.h>

//Freq mus in DS18x20.c angepasst werden!!!

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 16000000 //16MHz, why do i have to define the freq. in global_def instead of the top of main?
#endif

/******************************************************************************/
//Serial Communication
/******************************************************************************/

//Codes
/* debugging codes */
/*
#define ACK         97 //101 //Both: acknowledged
#define NACK        110 //Both: Not acknowledged. old: 102; new: 0 because 0 means nothing, easier to check

#define CPS         99 //RPI: Change Pump Speed
#define CHT         104 //RPI: Change Heating Temperature
#define CCT         105 //RPI: C1hange Cooling Temperature
#define CSS         106 //RPI: Change System State ON: values can be changed, OFF: values cant be changed

#define IWT         105//i, 201 //PIC: I wanna talk
#define IDWT        100//d, 202 //PIC: I dont wanna talk
#define IWR         82

#define FRAERR      70//204 //PIC: Framing Error, Restart Communication, last byte is lost
#define OVERERR     79//205 //PIC: Overrun Error, Restart Communication, last byte is lost
*/

/* production codes */
#define ACK         101 //101 //Both: acknowledged
#define NACK        102 //Both: Not acknowledged. old: 102; new: 0 because 0 means nothing, easier to check

#define CPS         103 //RPI: Change Pump Speed
#define CHT         104 //RPI: Change Heating Temperature
#define CCT         105 //RPI: C1hange Cooling Temperature
#define CSS         106 //RPI: Change System State ON: values can be changed, OFF: values cant be changed

#define IWT         201 //i, 201 //PIC: I wanna talk
#define IDWT        202 //d, 202 //PIC: I dont wanna talk
#define IWR         203 //RPI: I wanna read (the whole System State)

#define FRAERR      0//204 //PIC: Framing Error, Restart Communication, last byte is lost
#define OVERERR     0//205 //PIC: Overrun Error, Restart Communication, last byte is lost
 
 /**/
/******************************************************************************/
//PWM
/******************************************************************************/
#define DUTY_CYC_1  CCPR1L //set duty cycle for PWM1, between 0-255
#define DUTY_CYC_2  CCPR2L //set duty cycle for PWM2, between 0-255
#define DUTY_CYC_3  CCPR3L //set duty cycle for PWM3, between 0-255, connected to LED4: 0 LED on, 255: LED off


/******************************************************************************/
//Timer
/******************************************************************************/

#define CCP4_F PIR4bits.CCP4IF



#endif //GLOBAL_CONFIG_H
