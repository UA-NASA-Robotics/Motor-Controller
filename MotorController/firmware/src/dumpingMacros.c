#include "dumpingMacros.h"
#include "diggingMacros.h"
#include "actuatorControl.h"
#include "drivingControl.h"
#include "macroManagement.h"
#include "Motor.h"
#include "Timers.h"


#define DUMP_SPEED               -6000

#define startDrumDump()          setMotorVel(&DrumMotor, DUMP_SPEED)
#define stopDrumDump()           setMotorVel(&DrumMotor, 0)
#define MAX_Height  3900

timers_t DumpTimer;
void dumpingMacro(void)
{
    setTimerInterval(&DumpTimer,2500);
   driveDistance(-25, 2000);  
   setDiggingHeight(MAX_Height);
   startDrumDump();
 while(timerDone(&DumpTimer)  && continueMacro()) {
        updateMacroCommunications();
   }
   stopDrumDump();
   
   
   
        
}

