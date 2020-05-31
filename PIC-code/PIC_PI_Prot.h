#ifndef PIC_PI_PROT_H
#define PIC_PI_PROT_H

#include "global_Config.h"

uint8_t readValue();
void executeCommand(uint8_t, uint8_t); 
uint8_t SerStartEnd(uint8_t, uint16_t);
uint8_t ChangePumpSpeed(void);








#endif //PIC_PI_PROT_H