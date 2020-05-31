#ifndef SERCOM_H
#define SERCOM_H

#include "global_Config.h"

void SetupSerial(void);
//void CCPTimer1Setup(void); 
void DataFun(uint8_t); //does sth with the recData
//uint8_t SetTimer_us(uint16_t); //max 16000, sets 8bit timer. timer Flag will be set when determined time is reached.  returns 0 if input was >16000
//takes the time to set
//uint32_t ReadTimer_us(uint32_t, uint16_t); //reads the timer, if the flag is set it gives back the sum of all times since the loop startet
//if not it will give back zero. takes the sum of all times from bevore

void SerialSend(uint8_t data_par);
uint8_t SerialRec(void);


#endif //SERCOM_H



