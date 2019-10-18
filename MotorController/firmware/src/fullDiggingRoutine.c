#include "fullDiggingRoutine.h"
#include "diggingMacros.h"
#include "macroManagement.h"
#include "drivingControl.h"
#include "Timers.h"
#include <stdio.h>
#include "Motor.h"

#define PLOWING_DISTANCE 75

bool fullFlag = false;
int height;
timers_t DiggingTimer;
void fullDiggingRoutine(void) {
    height = DRUM_GROUND_HEIGHT;
    updateMacroCommunications();
    setTimerInterval(&DiggingTimer,60000);
    while (!timerDone(&DiggingTimer) && continueMacro() ) {
        
        /* Start Digging */
        height = diggingMacro(height);
        /* filter out the sand from the rocks */
        filterMaterials(30);
        /* Let's See if we are full of stuff*/
        //fullFlag = testDrumCapacityMacro();
        updateMacroCommunications();
    }
    MotorsAllStop();
}

