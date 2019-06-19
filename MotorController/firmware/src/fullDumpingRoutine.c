#include "fullDumpingRoutine.h"
#include "diggingMacros.h"
#include "plowControl.h"
#include "drivingControl.h"
#include "Timers.h"
#include "bucketArmControl.h"
#include "actuatorControl.h"
#include <stdio.h>

void fullDumpingRoutine(void)
{
     //Arm to Dump
    moveBucketArmToDump();
    //Actuator to dump
    moveActuatorToDump(3000);
    
    popLockAndDropIt();
    popLockAndDropIt();
    popLockAndDropIt();
    //Arm to Home
    moveBucketArmToHomeFromDump();
}