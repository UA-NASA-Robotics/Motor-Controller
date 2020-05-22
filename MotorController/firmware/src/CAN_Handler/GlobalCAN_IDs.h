/* 
 * File:   GlobalCAN_IDs.h
 * Author: John
 *
 * Created on November 15, 2019, 5:16 PM
 */

#ifndef GLOBALCAN_IDS_H
#define	GLOBALCAN_IDS_H
#include <stdint.h>
#ifdef	__cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include "CANFastTransfer.h"
#define GLOBAL_DEVICES 10
#define GLOBAL_DATA_IDX_PER_DEV 7
#ifndef GBL_ADDRESS
#define GBL_ADDRESS
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
            LIDAR,
    GLOBAL_ADDRESS = 31
} Addresses_t;
#endif
#ifndef GBL_DEV_IDX
#define GBL_DEV_IDX
//Complete index of all the
typedef enum {
	DEVICE_STATUS=0,
	DEVICE_MACRO,
	DATA_0,
	DATA_1,
	DATA_2,
	DATA_3,
	DATA_4
} GlobalDeviceData_t;
#endif

#define getGBL_DEVICE_STATUS(c) c*GLOBAL_DATA_IDX_PER_DEV + DEVICE_STATUS
#define getGBL_MACRO_INDEX(c) c*GLOBAL_DATA_IDX_PER_DEV + DEVICE_MACRO
#define getGBL_Data(c,d) c*GLOBAL_DATA_IDX_PER_DEV + d //c is device address, d is the index of the global data in that device's window

volatile short receiveArrayCAN_Global[GLOBAL_DEVICES*GLOBAL_DATA_IDX_PER_DEV +1];
volatile bool GBL_CAN_FT_recievedFlag[GLOBAL_DEVICES*GLOBAL_DATA_IDX_PER_DEV +1];

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBALCAN_IDS_H */

