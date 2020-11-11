
// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "uart_Handler.h"
#include "FastTransfer.h"
#include "actuatorControl.h"
#include "drivingControl.h"
#include "fullDiggingRoutine.h"
#include "diggingMacros.h"
#include "macroManagement.h"
#include "testingFunctions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Variable Declaration
// *****************************************************************************
// *****************************************************************************

APP_DATA appData;
timers_t sec, ms100, ms10;
timers_t bootTimer, ledTime;
bool isLoaded = false;
int val, test_jhs;



void APP_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    DRV_TMR0_Start();

    initChangeNotification();

    setTimerInterval(&sec, 1000);
    setTimerInterval(&ms100, 100);
    setTimerInterval(&ms10, 10);
    setTimerInterval(&ledTime, 75);
    setTimerInterval(&bootTimer, 3000);
    //while(1){LED1 ^=1;}
    while (!timerDone(&bootTimer)) {
        LED0 ^= 1;
        while (!timerDone(&ledTime));
        LED1 ^= 1;
        while (!timerDone(&ledTime));
        LED2 ^= 1;
        while (!timerDone(&ledTime));
        LED3 ^= 1;
        while (!timerDone(&ledTime));
    }


    initCANISRs();
    initCANFT();
    DRV_CAN0_Open();

    // initGlobalData(DEVICE_STATUS, getLoadedState, 1000);
    // initGlobalData(DEVICE_MACRO, getRunningMacros, 500);
    
    // TODO: remove test logic
    //initGlobalData(DATA_0, 56, 500);
    //TODO: next two lines for read
    test_jhs = getCANFastData(FT_GLOBAL, getGBL_Data(MOTOR_CONTROLLER, DATA_0));
    //printf("%d \n", test_jhs);

    initMotors();
    MotorsAllStop();

    //    InitUARTModule(&MasterUart,UART_Master);
    //    InitUARTModule(&GyroUart,UART_Gyro);

    //InitFastTransferModule(&MasterFT, UART_Master, MY_ADDRESS, Send_put, Buffer_Get, Buffer_Size, Buffer_Peek);
    //InitFastTransferModule(&GyroFT, UART_Gyro, MY_ADDRESS, Send_put, Buffer_Get, Buffer_Size, Buffer_Peek);


    delay(1000);

    //CALL THIS AFTER ZEROING STUFF
    initMacroCommunications();
    initMacroMGR();
    setMotorControlMode(&RightMotor, Velocity, 0);
    setMotorControlMode(&LeftMotor, Velocity, 0);
    isLoaded = true;

}
/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
timers_t TestTimer;

void APP_Tasks(void) {
    /* Check the application's current state. */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            if (appInitialized) {
                setTimerInterval(&TestTimer, 500);
                appData.state = APP_STATE_AWAITING_RESPONSE;
                //diggingMacro();
                //filterMaterials(30);
                //fullDiggingRoutine();
                //fullDiggingRoutine();
                //while(1);

            }
            break;
        }

        case TRANSMIT_GLOBAL_INFO:
        {

            publishData();
            LED0 ^= 1;

            appData.state = APP_STATE_RECEIVE_COMS;
            break;
        }
        case APP_STATE_RECEIVE_COMS:
        {
            // add any received macros to the macro queue
            handleMacroStatus();
            if (getRunningMacros() != 0) {
                runMacros();
            } else {
                if (getNewDataFlagStatus(FT_LOCAL, DRIVE_SPEED) || getNewDataFlagStatus(FT_LOCAL, ARM_SPEED) || getNewDataFlagStatus(FT_LOCAL, BUCKET_SPEED))
                    handleManualControl(getManualDriveSpeed(), getManualArmSpeed(), getManualBucketSpeed(), 0);

            }
            appData.state = APP_STATE_AWAITING_RESPONSE;
            if (getCANFastData(FT_GLOBAL, getGBL_Data(POZYX,DATA_0)) > 0) {
                LED0 ^= 1;
            }
            break;
        }
            //This is for waiting for an interrupt pin response
        case APP_STATE_AWAITING_RESPONSE:
        {
            //            int performMacro = false;
            //            short macroData = 0;
            //            if (receiveData(&MasterFT)) {
            //                performMacro = MasterFT.ReceivedData[8];
            //                macroData = (short) MasterFT.ReceivedData[9];
            //                //MasterFT.ReceivedData[8] = 0;
            //                MasterFT.ReceivedData[9] = 0;
            //                processMacro(performMacro, macroData);
            //            }

            appData.state = TRANSMIT_GLOBAL_INFO;
            break;
        }
            /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

int getLoadedState() {
    return (isLoaded ? 0x01 : 0x00);
}

/*******************************************************************************
 End of File
 */
