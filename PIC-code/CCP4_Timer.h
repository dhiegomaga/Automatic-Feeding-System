#ifndef CCP4_TIMER_H
#define CCP4_TIMER_H

#include "global_Config.h"


void CCP4_TimerSetup(void);
void CCP4_SetTimer(uint16_t);
void CCP4_Reset(void); //use only if its sure that all flags are set and need to be reset
//if not and it is called in a loop before set timer -> the code might not work



#endif //CCP4_TIMER_H