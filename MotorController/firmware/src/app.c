
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
int val;

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
        LED1 ^= 1;
        while (!timerDone(&ledTime));
        LED2 ^= 1;
        while (!timerDone(&ledTime));
        LED3 ^= 1;
        while (!timerDone(&ledTime));
        LED4 ^= 1;
        while (!timerDone(&ledTime));

    }

   
    initCANISRs();
    initCANFT();
    DRV_CAN0_Open();
    isLoaded = true;
    initGlobalData(Init_Element, getLoadedState, 500);

//    initMotors();
//    MotorsAllStop();

    //    InitUARTModule(&MasterUart,UART_Master);
    //    InitUARTModule(&GyroUart,UART_Gyro);

    InitFastTransferModule(&MasterFT, UART_Master, MY_ADDRESS, Send_put, Buffer_Get, Buffer_Size, Buffer_Peek);
    InitFastTransferModule(&GyroFT, UART_Gyro, MY_ADDRESS, Send_put, Buffer_Get, Buffer_Size, Buffer_Peek);


    delay(1000);


    //CALL THIS AFTER ZEROING STUFF
    initMacroCommunications();


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
                LED1 ^=1;
           
            appData.state = APP_STATE_RECEIVE_COMS;
            break;
        }
        case APP_STATE_RECEIVE_COMS:
        {
            
            appData.state = APP_STATE_AWAITING_RESPONSE;
            break;
        }
            //This is for waiting for an interrupt pin response
        case APP_STATE_AWAITING_RESPONSE:
        {
            int performMacro = false;
            short macroData = 0;
            if (receiveData(&MasterFT)) {
                performMacro = MasterFT.ReceivedData[8];
                macroData = (short) MasterFT.ReceivedData[9];
                //MasterFT.ReceivedData[8] = 0;
                MasterFT.ReceivedData[9] = 0;
                processMacro(performMacro, macroData);
            }
            if(ReceiveDataCAN(FT_GLOBAL))
            {
                
            }
            //            if (timerDone(&TestTimer))
            //                if (setDiggingHeight(3000))
            //                    LED4 ^= 1;

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

int getLoadedState()
{
    return (isLoaded ? 0x01 : 0x00);
}

/*******************************************************************************
 End of File
 */
