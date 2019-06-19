
#include <xc.h>
#include <stdio.h>
#include "actuatorControl.h"
#include "motorHandler.h"
#include "MotorDefinitions.h"
#include "Definitions.h"
#include "Timers.h"
#include "Motor.h"
#include "macroManagement.h"
#include "helperFunctions.h"
#include "bucketArmControl.h"

#define COUNTS_TO_LEVEL         -1300
#define TARGET_ACCEPTANCE_VALUE 20

timers_t sampleRate, debugTimer;

void displayActuatorSwitches(void);

void zeroActuator(void)
{
    moveActuatorToDump(2500);
    delay(500);
    diggingActuatorResetPositionData();
}

void sendActuatorToDig(int speed)
{    
    setMotorVel(&BucketActuator,-speed); 
}

void sendActuatorToDump(int speed)
{    
    setMotorVel(&BucketActuator,speed); 
}

void moveActuatorToCounts(int counts, int speed)
{
     //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    setTimerInterval(&debugTimer, 250);
    
    //Clear the motor fresh digital data flag
    getMotorDigitalInput(&BucketActuator,ACTUATOR_DUMP_DIGITAL);
    
    //Clear the motor fresh position data flag
    getMotorPosition(&BucketActuator);
    
    diggingActuatorRequestPositionData();
    diggingActuatorRequestDigitalData();   
    diggingActuatorContinueOperation();
    
    //Wait for fresh digital data
    while((!BucketActuator.FreshDigitals || !BucketActuator.FreshPosition) && continueMacro())
    {
        updateMacroCommunications();
    }
    
    while(!isAboutLong(getMotorPosition(&BucketActuator), counts,  TARGET_ACCEPTANCE_VALUE) && continueMacro())
    {
        updateMacroCommunications();
        if(timerDone(&sampleRate))
        {  
            diggingActuatorContinueOperation();
            //setMotorVel(&BucketActuator,-1000);
            if(getMotorPosition(&BucketActuator)>counts)
            {
                //setMotorVel(&BucketActuator,1000);
                sendActuatorToDig(speed);
            }
            else
            {                
                //setMotorVel(&BucketActuator,-1000);
                sendActuatorToDump(speed);
            }
            
            diggingActuatorRequestPositionData();
            diggingActuatorRequestDigitalData();     
            LED1 ^= 1;   
        }        
        if(timerDone(&debugTimer))
        {
            //printf("Counts: %ld\r", getMotorPosition(&BucketActuator));
        }
        //displayActuatorSwitches();
    }
    sendActuatorToDig(0);
}

void moveActuatorToLevelGround(int speed)
{ 
    moveActuatorToCounts(COUNTS_TO_LEVEL,speed);
}

void moveActuatorToFullDigging(int speed)
{
    //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    
    //Clear the motor fresh digital data flag
    getMotorDigitalInput(&BucketActuator,ACTUATOR_DUMP_DIGITAL);
    
    diggingActuatorRequestDigitalData();       
    diggingActuatorContinueOperation();
    
    //Wait for fresh digital data
    while(!BucketActuator.FreshDigitals&& continueMacro())
    {        
        updateMacroCommunications();
    }
    
    while(getMotorDigitalInput(&BucketActuator,ACTUATOR_DIG_DIGITAL) && continueMacro())
    {      
        updateMacroCommunications();
        if(timerDone(&sampleRate))
        {      
            diggingActuatorContinueOperation();
            //setMotorVel(&BucketActuator,-1000);
            sendActuatorToDig(speed);
            diggingActuatorRequestDigitalData();        
            LED1 ^= 1;   
        }        
        //displayActuatorSwitches();
    }   
    diggingActuatorContinueOperation();
}

void moveActuatorThroughDig(int speed)
{
    //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    
    //Clear the motor fresh digital data flag
    getMotorDigitalInput(&BucketActuator,ACTUATOR_DUMP_DIGITAL);
    
    diggingActuatorRequestDigitalData();      
    diggingActuatorContinueOperation();
    
    //Wait for fresh digital data
    while(!BucketActuator.FreshDigitals && continueMacro())
    {
        updateMacroCommunications();
    }
    
    while(getMotorDigitalInput(&BucketActuator,ACTUATOR_DUMP_DIGITAL)&& continueMacro())
    {        
        updateMacroCommunications();
        if(timerDone(&sampleRate))
        {    
            setMotorVel(&ArmMotor, 200);
            diggingActuatorContinueOperation(); 
            sendActuatorToDump(speed);
            diggingActuatorRequestDigitalData();         
            LED1 ^= 1;   
            LED2 ^= 1; 
        }
        //displayActuatorSwitches();
        //delay(100);
    }   
    diggingActuatorContinueOperation();
}

void moveActuatorToDump(int speed)
{
    //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    
    //Clear the motor fresh digital data flag
    getMotorDigitalInput(&BucketActuator,ACTUATOR_DUMP_DIGITAL);
    
    diggingActuatorRequestDigitalData();       
    diggingActuatorContinueOperation();
    
    //Wait for fresh digital data
    while(!BucketActuator.FreshDigitals && continueMacro())
    {        
        updateMacroCommunications();
    }
        
    while(getMotorDigitalInput(&BucketActuator,ACTUATOR_DUMP_DIGITAL)&& continueMacro())
    {      
        updateMacroCommunications();
        if(timerDone(&sampleRate))
        {      
            diggingActuatorContinueOperation();
            sendActuatorToDump(speed);
            diggingActuatorRequestDigitalData();       
            LED3 ^= 1;  
            LED4 ^= 1;  
        }
        //displayActuatorSwitches();
        //delay(100);
    }   
    diggingActuatorContinueOperation();
}

void twerkIt(void)
{
    diggingActuatorContinueOperation();
    if(continueMacro())
    setMotorVel(&BucketActuator,3000);  
    if(continueMacro())
    setMotorVel(&ArmMotor,3000);
    if(continueMacro())
    delay(300);
    
    diggingActuatorContinueOperation();   
    if(continueMacro())
    setMotorVel(&BucketActuator,-4000); 
    
    if(continueMacro())
    setMotorVel(&ArmMotor,-3000); 
    
    if(continueMacro())
    delay(300);
    
    diggingActuatorContinueOperation();   
    if(continueMacro())
    setMotorVel(&BucketActuator,3000);  
    
    if(continueMacro())
    setMotorVel(&ArmMotor,3000); 
    
    if(continueMacro())
    delay(300);
    diggingActuatorContinueOperation();
   
    if(continueMacro())
    setMotorVel(&BucketActuator,-4000);  
    
    if(continueMacro())
    setMotorVel(&ArmMotor,-3000); 
    
    if(continueMacro())
    delay(300);
    
    diggingActuatorContinueOperation();
    if(continueMacro())
    setMotorVel(&BucketActuator,0);  
    
    if(continueMacro())
    setMotorVel(&ArmMotor,0); 
    
}

void popLockAndDropIt(void)
{
    
    if(continueMacro())
    //sendMotorPacket(BUCKETMOTOR_STATUS,MOTOR_CONTINUE_OPERATION);   
    setMotorVel(&BucketActuator,3000); 
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);   
    if(continueMacro())
    setMotorVel(&ArmMotor,3000);
    moveBucketArmToDump();
    
    
    if(continueMacro())
    delay(300);
    setMotorVel(&BucketActuator,-3000); 
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);   
    if(continueMacro())        
    setMotorVel(&ArmMotor,-3000); 
   
    if(continueMacro())
    delay(300);
    
    if(continueMacro())  
    setMotorVel(&BucketActuator,3000); 
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);   
    if(continueMacro())
    setMotorVel(&ArmMotor,3000);     
    moveBucketArmToDump(); 
    
    if(continueMacro())
    delay(300);
    
    if(continueMacro())
    //sendMotorPacket(BUCKETMOTOR_STATUS,MOTOR_CONTINUE_OPERATION);   
    setMotorVel(&BucketActuator,-3000);  
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);   
    if(continueMacro())
    setMotorVel(&ArmMotor,-3000); 
    if(continueMacro())
    delay(300);
    //sendMotorPacket(BUCKETMOTOR_STATUS,MOTOR_CONTINUE_OPERATION);   
    setMotorVel(&BucketActuator,0);  
    setMotorVel(&ArmMotor,0); 
}

void displayActuatorSwitches(void)
{
    if(getMotorDigitalInput(&BucketActuator,1))               LED1 = 0;
    else                                                      LED1 = 1;
        
    if(getMotorDigitalInput(&BucketActuator,2))               LED2 = 0;
    else                                                      LED2 = 1;
        
    if(getMotorDigitalInput(&BucketActuator,3))               LED3 = 0;
    else                                                      LED3 = 1;
        
    if(getMotorDigitalInput(&BucketActuator,4))               LED4 = 0;
    else                                                      LED4 = 1;
}

void diggingActuatorResetPositionData(void)
{
    sendMotorPacket(ACTUATORMOTORID,MOTOR_ENCODER_POSITION_RESET);
}

void diggingActuatorRequestPositionData(void)
{
    requestMotorData(&BucketActuator, ENCODER_POSITION_REQUESTED); 
}

void diggingActuatorRequestDigitalData(void)
{
    requestMotorData(&BucketActuator, DIGITAL_INPUT_REQUESTED);       
}

void diggingActuatorContinueOperation(void)
{
    sendMotorPacket(ACTUATORMOTORID,MOTOR_CONTINUE_OPERATION);    
}