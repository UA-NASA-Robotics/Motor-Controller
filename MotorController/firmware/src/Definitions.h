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

#define RAD_TO_DEGREE   57.2957795

#define MY_ADDRESS     MOTOR_CONTROLLER //THE CAN FILTER WILL BE 0x7fc1 (Address 1)

#define UART_MACRO_RESPONCE

#define DRUM_DATA_INDEX     3

#define DrumLimitSwitch PORTGbits.RG9
#define DrumLS_ACTIVE   0
#define DrumLS_NotActive   1



#define CAN_COMMAND_INDEX      8
#define CAN_COMMAND_DATA_INDEX  9

//#define REVERSE_DRIVE_DIRECTION 
typedef struct{
    double x;
    double y;
}point_t;
#ifndef DATA_T
#define DATA_T

typedef struct {
    int data;
    bool newDataFlag;
} Data_t;




#endif
// FT_LOCAL CAN address
typedef enum{
    DRIVE_SPEED=1,
    ARM_SPEED=3,
    BUCKET_SPEED
}CANaddr_t;
#endif	/* DEFINITIONS_H */

