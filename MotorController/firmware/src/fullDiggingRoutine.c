#include "fullDiggingRoutine.h"
#include "diggingMacros.h"
#include "plowControl.h"
#include "drivingControl.h"
#include "Timers.h"
#include <stdio.h>

#define PLOWING_DISTANCE 75
void fullDiggingRoutine(void)
{
    //printf("Plowing\r");
    
    //Set plow above plowing height
    
    setPlowDepth(-1,4000);
    delay(1000);
    
    //Set to 0.25 inches depth
    setPlowDepth(0.25,2000);
    //Drive Forward
    driveDistance(PLOWING_DISTANCE,4000);
    //Move backwards
    //Retract the plow
    setPlowDepth(-1,4000);
    //Move backwards
    driveDistance(-PLOWING_DISTANCE,4000);
    
    //Set to 0.25 inches depth
    setPlowDepth(0.25,2000);
    //Drive Forward
    driveDistance(PLOWING_DISTANCE,4000);
    
    //Retract plow fully
    setPlowDepth(PLOW_FULL_UP,4000);
    //printf("Time To Dig\r");
    
    //Do some digging
    diggingMacro(false);    
}