#include "diggingMacros.h"
#include "actuatorControl.h"
#include "drivingControl.h"
#include "bucketArmControl.h"
#include "motorHandler.h"
#include "Motor.h"

void diggingMacro(bool b)
{
    if(b)
    {
        //zeroBucketArm();
//        zeroActuator();
        
        moveBucketArmToHomeFromDump();

        bucketArmResetPositionData();
    }
    
        
        //printf("movingActuatorToDig\r");
        //Actuator to digging angle
        //moveActuatorToFullDigging(4000);
        moveActuatorToLevelGround(2500);
        
       // printf("movingBucketToDig\r");
        //Arm to bottom
        moveBucketArmToDig();
        driveDistance(8,3000);
        //printf("movingActuatorThruDig\r");
        //Actuator up to dig
        moveActuatorThroughDig(2500);
       // printf("movingArmToHome\r");
        //Arm to Home
        moveBucketArmToHomeFromDig();
        //Shake shake shake...
        twerkIt();
}


void diggingMacro2(bool b)
{
    if(b)
    {
        //zeroBucketArm();
//        zeroActuator();
        
        moveBucketArmToHomeFromDump();

        bucketArmResetPositionData();
    }
    
        
        //printf("movingActuatorToDig\r");
        //Actuator to digging angle
        moveActuatorToFullDigging(4000);
        
       // printf("movingBucketToDig\r");
        //Arm to bottom
        moveBucketArmToDig2();
        //driveDistance(8,3000);
        //printf("movingActuatorThruDig\r");
        //Actuator up to dig
        moveActuatorThroughDig(2500);
       // printf("movingArmToHome\r");
        //Arm to Home
        moveBucketArmToHomeFromDig();
        //Shake shake shake...
        twerkIt();
}