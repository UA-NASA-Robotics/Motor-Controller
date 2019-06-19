#include "dumpingMacros.h"
#include "actuatorControl.h"
#include "bucketArmControl.h"

void dumpingMacro(void)
{
        //Arm to Dump
        moveBucketArmToDump();
        //Actuator to dump
        moveActuatorToDump(3000);
        //Move the stuff out
        twerkIt();
        twerkIt();
        //Arm to Home
        moveBucketArmToHomeFromDump();
        
}