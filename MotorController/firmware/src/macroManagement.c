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
#include "CAN_Handler/CANFastTransfer.h"
#include "Macro_Handler/Macro_Mgr.h"
#include <stdlib.h>
#include <stdio.h>
#define DRIVE_MACRO         (uint16_t)(1 << 10)
#define ARC_DRIVE_MACRO     4
#define DIGGING_MACRO       5
#define DUMPING_MACRO       6
#define dumbMac1             (uint16_t)(1 << 11)
#define dumbMac2            3
#define dumbMac3            9
#define dumbMac4            10
#define ZERO_SYSTEMS        100


PFI getMacroHandler(unsigned char _macroID);


timers_t voidTime,voidTime2;

bool Dummy(int val) {
    if (voidTime.timerInterval != val) {
        setTimerInterval(&voidTime, val);
    }
    return timerDone(&voidTime);
}
bool Dummy2(int val) {
    if (voidTime2.timerInterval != val) {
        setTimerInterval(&voidTime2, val);
    }
    return timerDone(&voidTime2);
}

void handleMacroStatus() {
    ReceiveDataCAN(FT_GLOBAL);

    /* If a macro is seen on the global bus from the router card*/
    if (getNewDataFlagStatus(FT_GLOBAL, getGBL_MACRO_INDEX(ROUTER_CARD))) {

        int macroID = getCANFastData(FT_GLOBAL, getGBL_MACRO_INDEX(ROUTER_CARD));
        int macroDATA = getCANFastData(FT_GLOBAL, getGBL_MACRO_INDEX(ROUTER_CARD) + 1);
        if (macroID == 0) {
            clearMacros();
        } else {
            /* Add the macro to the queue*/
            switch (macroID) {
                case DRIVE_MACRO:
                    setMacroCallback(Dummy, 5000, DRIVE_MACRO);
                    setMacroCallback(Dummy2, 15000, dumbMac1);
                    break;
                default:
                    break;
            }

        }
    }
    if (ReceiveDataCAN(FT_LOCAL)) {

    }

}





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
        case dumbMac1:
            break;
        case dumbMac2:
            break;
        case dumbMac3:
            break;
        case dumbMac4:
            break;
    }



}

void finishMacro(void) {
#ifndef UART_MACRO_RESPONCE
    togglePinState(&NaviPin1);

#else
    ToSend(&MasterFT, 1, 1);
    sendData(&MasterFT, MASTER_CONTROLLER);
#endif
}