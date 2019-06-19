#include <xc.h>
#include "bucketArmControl.h"
#include "actuatorControl.h"
#include "motorHandler.h"
#include "MotorDefinitions.h"
#include "Definitions.h"
#include "Timers.h"
#include "Motor.h"
#include "macroManagement.h"

#define COUNTS_CLOSE_TO_GROUND          250000
#define COUNTS_AT_GROUND                260000
#define COUNTS_OF_TOTAL_DIG             280000   //500000
//0 pos - dump
//2 neg - dig
#define BUCKET_ARM_SPEED        1500
#define DIGGING_DRIVE_SPEED     2500


timers_t sampleRate, debugTimer;

void displayArmMotorSwitches(void);
void bucketArmResetPositionData(void);
void bucketArmRequestPositionData(void);
void bucketArmRequestDigitalData(void);
void bucketArmSetMotorSpeed(int speed);
void bucketArmContinueOperation(void);

void zeroBucketArm(void)
{     
//    //Clear the motor fresh digital data flag
//    getMotorDigitalInput(&ArmMotor,ARM_HOME_DIGITAL);      
//    bucketArmRequestDigitalData(); 
//    //Wait for fresh data
//    while(!ArmMotor.FreshDigitals&& continueMacro())
//    {
//        updateMacroCommunications();        
//    }
//    
//    if(!getMotorDigitalInput(&ArmMotor,ARM_HOME_DIGITAL))
//    {
        moveBucketArmToDump();

        moveBucketArmToHomeFromDump();

        bucketArmResetPositionData();
//    }
}

void moveBucketArmToDig(void)
{    
    //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    setTimerInterval(&debugTimer, 250);
    
    //Create macro storage vars
    long    motorCounts     = 0;
    bool    treadsMoving    = false;
    bool    actuatorDigging = false;
    
    //Clear the motor fresh data flag
    getMotorPosition(&ArmMotor);
    
    //Ensure we are not stuck at the limit switch
    sendMotorPacket(ARMMOTORID          ,MOTOR_CONTINUE_OPERATION); 
    
    //Request new limit switch data
    bucketArmRequestDigitalData();     
    //Reset the position data
    bucketArmResetPositionData();
    //Request new position data
    bucketArmRequestPositionData(); 
    
    //Wait for fresh data
    while(!ArmMotor.FreshPosition && continueMacro())
    {
        updateMacroCommunications();
        delay(25);
    }
    
    //Store the latest motor position
    motorCounts = getMotorPosition(&ArmMotor);
    
//    printf("Counts: %ld\r", motorCounts);
//    printf("Position: %ld\r", getMotorPosition(&ArmMotor));
    
    //Start to move the motor
    bucketArmSetMotorSpeed(-BUCKET_ARM_SPEED);    
    
    while((motorCounts-getMotorPosition(&ArmMotor)) < (COUNTS_OF_TOTAL_DIG) && continueMacro()&&getMotorDigitalInput(&ArmMotor,ARM_DIG_DIGITAL))
    {        
        updateMacroCommunications();
        //if the treads are not moving, but we are on the ground with the bucket
        if((motorCounts-getMotorPosition(&ArmMotor))> COUNTS_CLOSE_TO_GROUND && !treadsMoving )
        {
            //Start driving
            sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);
            setMotor_Vel(DIGGING_DRIVE_SPEED,DIGGING_DRIVE_SPEED);
            bucketArmSetMotorSpeed(-BUCKET_ARM_SPEED/5);  
//            sendActuatorToDump(2000);
            //setMotorVel(&BucketActuator,-2000);            
            treadsMoving = true;
        }
        
        if(timerDone(&debugTimer))
        {
            //printf("Position: %ld\r", getMotorPosition(&ArmMotor));
        }
        
        //If the actuator is not moving, but the bucket is low enough to be moving
//        if(motorCounts-ArmMotor.Position > (COUNTS_AT_GROUND) && !actuatorDigging)
//        {
//            //sendActuatorToDump(2000);
//            sendMotorPacket(ACTUATORMOTORID          ,MOTOR_CONTINUE_OPERATION); 
//            setMotorVel(&BucketActuator,2500);
//            actuatorDigging=true;
//        }
        
        if(timerDone(&sampleRate))
        {            
//            setMotorVel(&ArmMotor,-BUCKET_ARM_SPEED); 
            bucketArmRequestPositionData(); 
            bucketArmRequestDigitalData(); 
            LED2 ^= 1;   
            LED3 ^= 1;
        }
    }
    
    bucketArmRequestDigitalData();
    
    //Stop the bucket arm
    bucketArmSetMotorSpeed(0);
    setMotorVel(&LeftMotor,0);
    setMotorVel(&RightMotor,0);
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);
}

void moveBucketArmToHomeFromDig(void)
{
    //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    
    //Clear the motor fresh digital data flag
    getMotorDigitalInput(&ArmMotor,ARM_HOME_DIGITAL);
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);   
    bucketArmRequestDigitalData(); 
    
    //Wait for fresh data
    while(!ArmMotor.FreshDigitals&& continueMacro())
    {
        updateMacroCommunications();
    }
    
    setMotorVel(&ArmMotor,BUCKET_ARM_SPEED); 
    
    while(getMotorDigitalInput(&ArmMotor,ARM_HOME_DIGITAL) && continueMacro())
    {        
        updateMacroCommunications();
        if(timerDone(&sampleRate))
        {
            setMotorVel(&ArmMotor,BUCKET_ARM_SPEED); 
            requestMotorData(&ArmMotor, DIGITAL_INPUT_REQUESTED);  
            LED4 ^= 1;
        }  
        //displayArmMotorSwitches(); 
        //delay(100);
    }   
    setMotorVel(&ArmMotor,0); 
}
void moveBucketArmToHomeFromDump(void)
{
    //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    
    //Clear the motor fresh digital data flag
    getMotorDigitalInput(&ArmMotor,ARM_HOME_DIGITAL);
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);    
    requestMotorData(&ArmMotor, DIGITAL_INPUT_REQUESTED); 
    setMotorVel(&ArmMotor,-BUCKET_ARM_SPEED); 
    
    //Wait for fresh data
    while(!ArmMotor.FreshDigitals&& continueMacro())
    {
        updateMacroCommunications();
    }
    
    while(getMotorDigitalInput(&ArmMotor,ARM_HOME_DIGITAL) && continueMacro())
    {            
        updateMacroCommunications();
        if(timerDone(&sampleRate))
        {    
            sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION); 
            setMotorVel(&ArmMotor,-BUCKET_ARM_SPEED); 
            requestMotorData(&ArmMotor, DIGITAL_INPUT_REQUESTED);  
            LED1 ^= 1;   
            LED4 ^= 1;     
        }
        //displayArmMotorSwitches(); 
        //delay(100);
    }   
    setMotorVel(&ArmMotor,0); 
}

void moveBucketArmToDump(void)
{
    //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    
    //Clear the motor fresh digital data flag
    getMotorDigitalInput(&ArmMotor,ARM_HOME_DIGITAL);
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);
    requestMotorData(&ArmMotor, DIGITAL_INPUT_REQUESTED); 
    setMotorVel(&ArmMotor,BUCKET_ARM_SPEED); 
    
    //Wait for fresh data
    while(!ArmMotor.FreshDigitals&& continueMacro())
    {
        updateMacroCommunications();        
    }
    
    while(getMotorDigitalInput(&ArmMotor,ARM_DUMP_DIGITAL) && continueMacro())
    {                 
        updateMacroCommunications();
        if(timerDone(&sampleRate))
        {    
            sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);
            setMotorVel(&ArmMotor,BUCKET_ARM_SPEED);
            requestMotorData(&ArmMotor, DIGITAL_INPUT_REQUESTED);    
        }
        //displayArmMotorSwitches();
        //delay(100);
    }   
}


void displayArmMotorSwitches(void)
{
    if(getMotorDigitalInput(&ArmMotor,1))                    LED1 = 0;
    else                                                     LED1 = 1;

    if(getMotorDigitalInput(&ArmMotor,2))                    LED2 = 0;
    else                                                     LED2 = 1;

    if(getMotorDigitalInput(&ArmMotor,3))                    LED3 = 0;
    else                                                     LED3 = 1;

    if(getMotorDigitalInput(&ArmMotor,4))                    LED4 = 0;
    else                                                     LED4 = 1;
}

void bucketArmResetPositionData(void)
{
    sendMotorPacket(ARMMOTORID,SSI_ENCODER_POSITION_RESET);
}

void bucketArmRequestPositionData(void)
{
    requestMotorData(&ArmMotor, SSI_ENCODER_POSITION_REQUESTED); 
}

void bucketArmRequestDigitalData(void)
{
    requestMotorData(&ArmMotor, DIGITAL_INPUT_REQUESTED);       
}

void bucketArmSetMotorSpeed(int speed)
{
    setMotorVel(&ArmMotor,speed);    
}

void bucketArmContinueOperation(void)
{
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION); 
}




void moveBucketArmToDig2(void)
{    
    //Setup the timer system
    setTimerInterval(&sampleRate, 100);
    setTimerInterval(&debugTimer, 250);
    
    //Create macro storage vars
    long    motorCounts     = 0;
    bool    treadsMoving    = false;
    bool    actuatorDigging = false;
    
    //Clear the motor fresh data flag
    getMotorPosition(&ArmMotor);
    
    //Ensure we are not stuck at the limit switch
    sendMotorPacket(ARMMOTORID          ,MOTOR_CONTINUE_OPERATION); 
    
    //Request new limit switch data
    bucketArmRequestDigitalData();     
    //Reset the position data
    bucketArmResetPositionData();
    //Request new position data
    bucketArmRequestPositionData(); 
    
    //Wait for fresh data
    while(!ArmMotor.FreshPosition && continueMacro())
    {
        updateMacroCommunications();
        delay(25);
    }
    
    //Store the latest motor position
    motorCounts = getMotorPosition(&ArmMotor);
    
//    printf("Counts: %ld\r", motorCounts);
//    printf("Position: %ld\r", getMotorPosition(&ArmMotor));
    
    //Start to move the motor
    bucketArmSetMotorSpeed(-BUCKET_ARM_SPEED);    
    
    while((motorCounts-getMotorPosition(&ArmMotor)) < (500000) && continueMacro() && getMotorDigitalInput(&ArmMotor,ARM_DIG_DIGITAL))
    {        
        updateMacroCommunications();
        //if the treads are not moving, but we are on the ground with the bucket
//        if((motorCounts-getMotorPosition(&ArmMotor))> COUNTS_CLOSE_TO_GROUND && !treadsMoving )
//        {
//            //Start driving
//            sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);
//            setMotor_Vel(DIGGING_DRIVE_SPEED,DIGGING_DRIVE_SPEED);
//            bucketArmSetMotorSpeed(-BUCKET_ARM_SPEED/5);  
////            sendActuatorToDump(2000);
//            //setMotorVel(&BucketActuator,-2000);            
//            treadsMoving = true;
//        }
//        
        if(timerDone(&debugTimer))
        {
            //printf("Position: %ld\r", getMotorPosition(&ArmMotor));
        }
        
        //If the actuator is not moving, but the bucket is low enough to be moving
//        if(motorCounts-ArmMotor.Position > (COUNTS_AT_GROUND) && !actuatorDigging)
//        {
//            //sendActuatorToDump(2000);
//            sendMotorPacket(ACTUATORMOTORID          ,MOTOR_CONTINUE_OPERATION); 
//            setMotorVel(&BucketActuator,2500);
//            actuatorDigging=true;
//        }
        
        if(timerDone(&sampleRate))
        {            
//            setMotorVel(&ArmMotor,-BUCKET_ARM_SPEED); 
            bucketArmRequestPositionData(); 
            bucketArmRequestDigitalData(); 
            LED2 ^= 1;   
            LED3 ^= 1;
        }
    }
    
    bucketArmRequestDigitalData();
    
    //Stop the bucket arm
    bucketArmSetMotorSpeed(0);
    setMotorVel(&LeftMotor,0);
    setMotorVel(&RightMotor,0);
    
    sendMotorPacket(ARMMOTORID,MOTOR_CONTINUE_OPERATION);
    sendMotorPacket(ACTUATORMOTORID,MOTOR_CONTINUE_OPERATION);
}