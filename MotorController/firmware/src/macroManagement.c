#include "macroManagement.h"
#include "drivingControl.h"
#include "changeNotification.h"
#include "fullDiggingRoutine.h"
#include "fullDumpingRoutine.h"
#include "FastTransfer.h"
#include "plowControl.h"
#include "diggingMacros.h"
#include "actuatorControl.h"
#include "bucketArmControl.h"
#include "Definitions.h"

#include "Timers.h"

#define DRIVE_MACRO         3
#define PLOW_MACRO          4
#define DIGGING_MACRO       5
#define DUMPING_MACRO       6
#define ZERO_SYSTEMS        100


void finishMacro(void);

bool continueTheMacro = true;
bool continueMacro(void)
{
    return continueTheMacro;
}

void initMacroCommunications(void)
{
    continueTheMacro = false;
    MasterFT.ReceivedData[8]=0;
}

void updateMacroCommunications(void)
{
    receiveData(&MasterFT);
    if(MasterFT.ReceivedData[8]==0)
    {
        continueTheMacro=false;
    }
}

void processMacro(uint8_t performMacro, int macroData)
{ 
    continueTheMacro = true;
    switch(performMacro)
    {
        case DRIVE_MACRO:
            driveDistance(macroData,3500);
            break;
        case DIGGING_MACRO:            
            diggingMacro2(macroData); 
            break;
        case DUMPING_MACRO:
            fullDumpingRoutine();
            break;
        case PLOW_MACRO:
            //doPlowMacro(75);
            doPlowMacro2(75);
            break;
        case ZERO_SYSTEMS:
             zeroPlowDepth();  
             zeroBucketArm();
             zeroActuator();
            break;
    }
    
    if(MasterFT.ReceivedData[8] == 0)
    {      
        LED1 =0;
        LED2 =0;
        LED3 =0;
        LED4 =0;  
        
       LED1 ^=1;
       delay(50);
        LED2 ^=1;
       delay(50);
        LED3 ^=1;
       delay(50);
        LED4 ^=1;
       delay(50);   
        
       LED1 ^=1;
       delay(50);
        LED2 ^=1;
       delay(50);
        LED3 ^=1;
       delay(50);
        LED4 ^=1;
       delay(50);
       
       LED1 ^=1;       
       delay(50);
        LED2 ^=1;
       delay(50);
        LED3 ^=1;
       delay(50);
        LED4 ^=1;
       delay(50);
    }
    else
    {
        
        LED1 =0;
        LED2 =0;
        LED3 =0;
        LED4 =0;
        delay(1000);
    }
    MasterFT.ReceivedData[8] = 0;
    finishMacro();
    //Send completed
    //OUTPUT_INTURRPT_PIN_3^=1;

}

void finishMacro(void)
{
   
    ToSend(&MasterFT,1,1);
    sendData(&MasterFT,MASTER_ADDRESS);
}