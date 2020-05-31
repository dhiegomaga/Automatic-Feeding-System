/*
 * File:   SerCom.c
 * Author: Megaport
 *
 * Created on 24. Mai 2020, 19:07
 */


#include "SerCom.h"

void SetupSerial(void){
    //RC7_RX, RC6_TX, x=1
    
    //Enable Transmitter RC6_TX
    ANSELCbits.ANSC6 = 0; //digital
    TRISCbits.RC6 = 1; //input
    
    TXSTA1bits.TXEN = 1; //enables transmitter
    TXSTA1bits.SYNC = 0; //config for async op
    RCSTA1bits.SPEN = 0; //1: enables EUSART and config Rx/Tx
    
    
    //Enable Receiver RC7_RX
    ANSELCbits.ANSC7 = 0; //digital
    TRISCbits.RC7 = 1; //input
    
    RCSTA1bits.CREN = 1;  //enables receiver circuit
    //TXSTAxbits.SYNC = 0 enables async mode //muss nur einaml gesetzt werden wenn ich receiver und transmitter nutz
    //RCSTAxbits.SPEN = 1 enables eusart //muss nur 1 mal gesetzt werden

    //Baudgenerator
    //9600baud at FOSC = 16MHz, err = 0,08%
    //BAUDCON1bits.BRG16 = 1;
    BAUDCON1bits.BRG16 = 0;
    TXSTA1bits.BRGH = 1;
    SPBRG1 = 103;
    //SPBRGH1:SPBRG1 = 416;
    //SPBRGH1 = 0x01;
    //SPBRG1 = 0xA0; //if that doesnt work use the 9600 baud version with err = 0,16%
    //9600baud	err=0,16%	SPBRGx = 103	SYNC = 0	BRGH = 1	BRG16 = 0
    
}



//Function that does sth with the received data

void DataFun(uint8_t data){
   
   return;
}

//if needed perfor a type conversion
void SerialSend(uint8_t data_par){
        while(!(PIR1bits.TX1IF)); //if data is currently sent, wait till it finishes, 1: buffer empty, 0: full
        
        TXREG1 = data_par;//Send Data 
        __delay_us(1); 
}

uint8_t SerialRec(void){
    uint8_t recData_intern = NACK; //value for no data Recived = default value
    if(PIR1bits.RC1IF == 1){ //data in FIFO
               __delay_us(1);
               if(RCSTA1bits.OERR == 0){ //no Overrun error, if Overrun happened DataFun() shouldnt be called
                    __delay_us(1);
                    if(RCSTA1bits.FERR == 0){ //no Framing Error
                        recData_intern = RCREG1; //read receive register
                        __delay_us(1);
                    }else{ //Framing Error
                        SerialSend(FRAERR); //send Framing error code
                        RCSTA1bits.SPEN = 0;  //reset eusart
                        __delay_us(10);
                        RCSTA1bits.SPEN = 1;
                    }
               } else{ //overrun error
                 SerialSend(OVERERR);//send Overrun error code
                 //RCSTA1bits.SPEN = 0;  //reset eusart, clear everything
                 RCSTA1bits.CREN = 0;
                 __delay_ms(1);
                 //RCSTA1bits.SPEN = 1; 
                 RCSTA1bits.CREN = 1;
               }
            } //Data in FIFO
    
    return recData_intern;
    
}






