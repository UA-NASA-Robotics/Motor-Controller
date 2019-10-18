#include "testingFunctions.h"
#include "Definitions.h"
#include "Timers.h"
#include "MotorDefinitions.h"
#include "Motor.h"
#include "actuatorControl.h"
#include "drivingControl.h"

#include "macroManagement.h"
#include "fullDiggingRoutine.h"
#include "diggingMacros.h"


void testMotorControlIntelligently(void)
{
    setMotorVel(&LeftMotor,4000);
    setMotorVel(&RightMotor,4000);
    while(!(LeftMotor.Position>80000 && RightMotor.Position>80000 ))
    {
        LATE ^= 0xF0;
        delay(100);
                      
        requestMotorData(&LeftMotor, SSI_ENCODER_POSITION_REQUESTED);
        requestMotorData(&RightMotor, SSI_ENCODER_POSITION_REQUESTED);
    }
    setMotorVel(&LeftMotor,0);
    setMotorVel(&RightMotor,0);   
}

void testMotorDigitalOutput(void)
{
    bool outputSet=true;
    while(1)
    {
        setMotorDigitalOutput(&DrumMotor, 1, outputSet);   
        outputSet ^= 1;
        delay(1000);
        LED1 ^= 1;        
    }
}

void testArmMotorDigitalInputs(void)
{
    while(1)
    {
        requestMotorData(&ArmMotor, DIGITAL_INPUT_REQUESTED);        
        delay(100);
        
        if(getMotorDigitalInput(&ArmMotor,1))               LED1 = 1;
        else                                                LED1 = 0;
        
        if(getMotorDigitalInput(&ArmMotor,2))               LED2 = 1;
        else                                                LED2 = 0;
        
        if(getMotorDigitalInput(&ArmMotor,3))               LED3 = 1;
        else                                                LED3 = 0;
        
        if(getMotorDigitalInput(&ArmMotor,4))               LED4 = 1;
        else                                                LED4 = 0;       
    }   
}

void testActuatorMotor(void)
{
    setMotorVel(&ArmMotor,200); 
    while(1)
    {
        
        while(getMotorDigitalInput(&DrumMotor,ACTUATOR_DUMP_DIGITAL))
        {        
            requestMotorData(&DrumMotor, DIGITAL_INPUT_REQUESTED);     
            LED1 ^= 1;   
            delay(100);
        }
        
        setMotorVel(&ArmMotor,200); 
        
        while(getMotorDigitalInput(&DrumMotor,ACTUATOR_DIG_DIGITAL))
        {        
            requestMotorData(&DrumMotor, DIGITAL_INPUT_REQUESTED);     
            LED1 ^= 2;   
            delay(100);
        }
        
        setMotorVel(&ArmMotor,-200); 
        
        
    }
}

void testBucketActuator(void)
{
    setMotorVel(&DrumMotor,200); 
    while(1)
    {
        LED1 ^= 1;   
        delay(100);
        
    }
}

void testDiggingFunction(void)
{
    while(1)
    {        
        
    }
}
 
void testMotorDistanceCommand(void)
{
    //Send a cm and a speed in rpm
    driveDistance(50,2500); //55
}
void testMotorDistanceVariedSpeedsCommand(void)
{    
    //Send a cm and a speed in rpm
    driveDistanceVariedSpeed(50,2500); //55
}

void testMotorDataCollector(void)
{
    while(1)
    {        
//        requestMotorData(&BucketActuator    , DIGITAL_INPUT_REQUESTED); 
//        requestMotorData(&ArmMotor          , DIGITAL_INPUT_REQUESTED); 
        requestMotorData(&ArmMotor          , SSI_ENCODER_POSITION_REQUESTED); 
        printf("ArmMotor Pos: %ld", ArmMotor.Position);
//        requestMotorData(&LeftMotor         , SSI_ENCODER_POSITION_REQUESTED); 
//        requestMotorData(&RightMotor        , SSI_ENCODER_POSITION_REQUESTED); 
        delay(100);
    }
}

void testAnalogFeedbackPlowMotor(void)
{
    while(1)
    {
        requestMotorData(&PlowMotor,ANALOG_0_REQUESTED);
        requestMotorData(&PlowMotor,ANALOG_1_REQUESTED);
        printf("Analog0Value: %d\r\n",PlowMotor.Analog0);
        printf("Analog1Value: %d\r\n",PlowMotor.Analog1);
        delay(1000);
    }    
}

void testPlowMovement(void)
{
   
}

void testPlowingFunction(void)
{
    while(1)
    {
        
        fullDiggingRoutine();
    }
}