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

typedef enum {
    CONTROLBOX = 1,
    POZYX,
    JUICE_BOARD,
    ROUTER_CARD,
    MASTER_CONTROLLER,
    MOTOR_CONTROLLER,
    GYRO_CONTROLLER,
    STRAIN_SENSOR,
    OPTICAL_FLOW,
    RASPBERRY_PI,
    LED_CARD,
    GLOBAL_ADDRESS = 31
} Addresses_t;

#define MY_ADDRESS     MOTOR_CONTROLLER //THE CAN FILTER WILL BE 0x7fc1 (Address 1)

#define UART_MACRO_RESPONCE

#define DRUM_DATA_INDEX     3

#define DrumLimitSwitch PORTGbits.RG9
#define DrumLS_ACTIVE   0
#define DrumLS_NotActive   1

#define REVERSE_DRIVE_DIRECTION 


#endif	/* DEFINITIONS_H */

