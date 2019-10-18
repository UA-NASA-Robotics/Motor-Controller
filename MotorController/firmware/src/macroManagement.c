#include "macroManagement.h"
#include "drivingControl.h"
#include "changeNotification.h"
#include "fullDiggingRoutine.h"
#include "fullDumpingRoutine.h"
#include "FastTransfer.h"
#include "diggingMacros.h"
#include "actuatorControl.h"
#include "Definitions.h"
#include "ArcDriveMacro.h"

#include "Timers.h"

#define DRIVE_MACRO         3
#define ARC_DRIVE_MACRO     4
#define DIGGING_MACRO       5
#define DUMPING_MACRO       6
#define dumbMac1            7
#define dumbMac2            8
#define dumbMac3            9
#define dumbMac4            10



#define ZERO_SYSTEMS        100


void finishMacro(void);

bool continueTheMacro = false;

bool continueMacro(void) {
    return continueTheMacro;
}

void setContinueMacroState(bool state) {
    continueTheMacro = state;
}

void initMacroCommunications(void) {
    continueTheMacro = false;
    MasterFT.ReceivedData[8] = 0;
}

void updateMacroCommunications(void) {
    receiveData(&MasterFT);
    if (MasterFT.ReceivedData[8] == 0) {
        continueTheMacro = false;
    }
}

void processMacro(uint8_t performMacro, int macroData) {
    continueTheMacro = true;
    switch (performMacro) {
        case DRIVE_MACRO:
            driveDistance(macroData, 1500);
            break;
        case DIGGING_MACRO:
            //diggingMacro2(macroData); 
            fullDiggingRoutine();
            break;
        case DUMPING_MACRO:
            fullDumpingRoutine();
            break;
        case ARC_DRIVE_MACRO:
            ArcDrive(MasterFT.ReceivedData[9], MasterFT.ReceivedData[10], MasterFT.ReceivedData[11], MasterFT.ReceivedData[12], MasterFT.ReceivedData[13]);
            break;
        case ZERO_SYSTEMS:

            break;
        case dumbMac1: //Drive Strait

            driveDistance(506, 2500);
            fullDiggingRoutine();
            finishMacro();

            break;
        case dumbMac2: //Turn counterClockwise 90 degrees then drive

            driveDistance(506, 2500);
            fullDiggingRoutine();
            finishMacro();

            break;
        case dumbMac3: //trun clockwise 90 degrees then drive

            driveDistance(506, 2500);
            fullDiggingRoutine();
            finishMacro();

            break;
        case dumbMac4: //turn 180 then drive 90 degrees

            driveDistance(506, 2500);
            fullDiggingRoutine();
            finishMacro();
            break;
    }

    if (MasterFT.ReceivedData[8] == 0) {
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;

        LED1 ^= 1;
        delay(50);
        LED2 ^= 1;
        delay(50);
        LED3 ^= 1;
        delay(50);
        LED4 ^= 1;
        delay(50);

        LED1 ^= 1;
        delay(50);
        LED2 ^= 1;
        delay(50);
        LED3 ^= 1;
        delay(50);
        LED4 ^= 1;
        delay(50);

        LED1 ^= 1;
        delay(50);
        LED2 ^= 1;
        delay(50);
        LED3 ^= 1;
        delay(50);
        LED4 ^= 1;
        delay(50);
    } else {

        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        delay(1000);
    }
    MasterFT.ReceivedData[8] = 0;
    finishMacro();
    //Send completed
    //OUTPUT_INTURRPT_PIN_3^=1;

}

void finishMacro(void) {
#ifndef UART_MACRO_RESPONCE
    togglePinState(&NaviPin1);

#else
    ToSend(&MasterFT, 1, 1);
    sendData(&MasterFT, MASTER_ADDRESS);
#endif
}