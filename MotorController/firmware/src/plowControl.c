#include "plowControl.h"
#include "motorHandler.h"
#include "MotorDefinitions.h"
#include "Definitions.h"
#include "Timers.h"
#include "Motor.h"
#include "macroManagement.h"
#include "helperFunctions.h"
#include "drivingControl.h"

#define PLOW_MOVEMENT_UP            -1
#define PLOW_MOVEMENT_DOWN          1    
#define PLOW_MOVEMENT_SPEED_HOMING  1000
#define PLOW_MOVEMENT_SPEED_PLOWING 1000
#define GROUND_ANALOG_VALUE         2500 

#define SCALING_FACTOR_CONVERSION 600
#define TARGET_ACCEPTANCE_VALUE 25
timers_t debugTimer, requestTimer;

void initTimersPlow(void)
{
    setTimerInterval(&requestTimer,50);
}

//SEND THE PLOW TO ZERO DEPTH
void zeroPlowDepth(void)
{ 
    setPlowDepth(-4,4000);
}

//SET THE PLOW DEPTH TO A VALUE IN 'INCHES' FROM 0->5
//uint16_t analogRead=0;
void setPlowDepth(float depthInInches, int plowSpeed)
{
    setTimerInterval(&debugTimer,100);
    setTimerInterval(&requestTimer,50);
    bool speedNotSet=true;
    bool locomotionNotMoving=true;
    int16_t analogRead=0;
//    analogRead=0;
    
    //Request new analog data
    requestMotorData(&PlowMotor,ANALOG_0_REQUESTED);  
    
    //Wait for new analog data to be rx-ed
    while((analogRead==0) && continueMacro())
    {
        updateMacroCommunications();
        analogRead = getMotorAnalog0(&PlowMotor);
//        analogRead = PlowMotor.Analog0;
        //printf("ARead: %u\r", analogRead);
        delay(50);
    }
    
    //calculate the analog data we want to see from the plow
    int analogSet=((depthInInches+4)*SCALING_FACTOR_CONVERSION) + MINIMUM_ANALOG_POSITION;
        
    //While the plow is not in the target range
    while( !isAboutInt(analogRead, analogSet, TARGET_ACCEPTANCE_VALUE) && continueMacro())
    {
        updateMacroCommunications();
        
        if(timerDone(&debugTimer))
        {
            //printf("ARead:%d, Set: %d\r",analogRead,analogSet);
            
            //REQUEST A NEW VALUE OF THE ANALOG FEEDBACK
            requestMotorData(&PlowMotor,ANALOG_0_REQUESTED);
        }
        
        if(timerDone(&requestTimer))
        {            
            
            sendMotorPacket(PLOWMOTORID,MOTOR_CONTINUE_OPERATION);    
            //STORE THE VALUE OF THE ANALOG FEEDBACK
            analogRead = getMotorAnalog0(&PlowMotor);
//            analogRead = PlowMotor.Analog0;
            
            if(analogRead>GROUND_ANALOG_VALUE && locomotionNotMoving && analogSet>GROUND_ANALOG_VALUE)
            {
                setMotor_Vel(4000,4000);
                locomotionNotMoving = false;
            }
//            //REQUEST A NEW VALUE OF THE ANALOG FEEDBACK
//            requestMotorData(&PlowMotor,ANALOG_0_REQUESTED);

            //SEND A SPEED TO THE PLOW FOR MOVEMENT
//            setMotorVel(&PlowMotor,speedSetting);
            if(speedNotSet)
            {
                if(analogRead<analogSet)
                {
                   setMotorVel(&PlowMotor,plowSpeed);
                }
                if(analogRead>analogSet)
                {
                   setMotorVel(&PlowMotor,-plowSpeed);
                }
                speedNotSet=false;
            }
        }
    }
    setMotorVel(&PlowMotor,0);
    setMotorVel(&LeftMotor,0);
    setMotorVel(&RightMotor,0);
}
#define NUM_PLOW 2


//PLOW, TOPPLE PILE, MOVE OVER THE PLOW AREA, SCOOP, DISCARD, SCOOP, twerk, scoop.
void doPlowMacro(int distance)
{
//    for(i = 0; i< NUM_PLOW;i++)
//    {
//        

        //Set to 0.25 inches depth
       setPlowDepth(0.3,3000);
       //Drive Forward
       driveDistance(100,5000);
       //Move backwards
       //Retract the plow
       setPlowDepth(-1,4000);
       //Move backwards
       driveDistance(-100,5000);

       //Set to 0.25 inches depth
       setPlowDepth(0.25,3000);
       //Drive Forward
       driveDistance(100,5000);

       //Retract plow fully
       setPlowDepth(PLOW_FULL_UP,4000);
  //  }
}

void doPlowMacro2(int distance)
{
//    for(i = 0; i< NUM_PLOW;i++)
//    {
//        

        //Set to 0.25 inches depth
       setPlowDepth(0.3,5000);
       //Drive Forward
       driveDistance(100,5000);
       //DRAG BACKWARDSdepth
       setPlowDepth(0.45,5000);
       //Move backwards
       driveDistance(-75,5000);

       //Set to 0.25 inches depth
       setPlowDepth(0,5000);
       //Drive Forward
       driveDistance(100,5000);

       //Retract plow fully
       setPlowDepth(PLOW_FULL_UP,4000);
  //  }
}