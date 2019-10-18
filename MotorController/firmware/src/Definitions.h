/* 
 * File:   Definitions.h
 * Author: John
 *
 * Created on April 27, 2018, 6:32 PM
 */

#ifndef DEFINITIONS_H
#define	DEFINITIONS_H


#define LED1 LATEbits.LATE5
#define LED2 LATEbits.LATE6
#define LED3 LATEbits.LATE7
#define LED4 LATGbits.LATG6

#define MY_ADDRESS      6 //THE FILTER WILL BE 0x7fc1 (Address 1)

#define MASTER_ADDRESS 5

#define MACRO_COMMAND_INDEX 1
#define MACRO_COMMAND_DATA_INDEX 2

#define UART_MACRO_RESPONCE

#define DRUM_DATA_INDEX     3

#define DrumLimitSwitch PORTGbits.RG9
#define DrumLS_ACTIVE   0
#define DrumLS_NotActive   1

#define REVERSE_DRIVE_DIRECTION 


#endif	/* DEFINITIONS_H */

