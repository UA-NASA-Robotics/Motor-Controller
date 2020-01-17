/* 
 * File:   Definitions.h
 * Author: John
 *
 * Created on April 27, 2018, 6:32 PM
 */

#ifndef DEFINITIONS_H
#define	DEFINITIONS_H
#include <xc.h>
#include "CAN_Handler/GlobalCAN_IDs.h"

#define LED1 LATEbits.LATE5
#define LED2 LATEbits.LATE6
#define LED3 LATEbits.LATE7
#define LED4 LATGbits.LATG6

#define MY_ADDRESS     MOTOR_CONTROLLER //THE CAN FILTER WILL BE 0x7fc1 (Address 1)

#define UART_MACRO_RESPONCE

#define DRUM_DATA_INDEX     3

#define DrumLimitSwitch PORTGbits.RG9
#define DrumLS_ACTIVE   0
#define DrumLS_NotActive   1

//#define REVERSE_DRIVE_DIRECTION 

#ifndef DATA_T
#define DATA_T

typedef struct {
    int data;
    bool newDataFlag;
} Data_t;


// FT_LOCAL CAN address
typedef enum{
    DRIVE_SPEED=1,
    ARM_SPEED=3,
    BUCKET_SPEED
}CANaddr_t;
#endif

#endif	/* DEFINITIONS_H */

