#include "diggingMacros.h"
#include <stddef.h>
#include "system_config/default/system_config.h"
#include "actuatorControl.h"
#include "drivingControl.h"
#include "motorHandler.h"
#include "PID.h"
#include "Motor.h"
#include "macroManagement.h"
#include "Communications.h"
#include "Definitions.h"
#include "CANFastTransfer.h"



int16_t getDrumheight();
bool HeightTolerance(int target, int actualValue);
int initDiggingPID(int _target, int currentValue);
PID_Struct_t ActuatorPID;
int newDrumHeight = 0;
timers_t DrumIncTimer, DigTimer, filterTimer,stallTimer;

int initDiggingPID(int _target, int currentValue) {
    if (ActuatorPID._target != _target) {
        INIT_PID(&ActuatorPID, _target, 2, .01, 0);
    }
    return updateOutput(&ActuatorPID, currentValue);
}
// Returns the deepest digging depth reached!!!

int diggingMacro(int startDepth) {
    //setContinueMacroState(true);
    setTimerInterval(&DrumIncTimer, INC_DRUM_HEIGHT_TIME);
    startDrumDig();
    setDiggingHeight(startDepth);
    newDrumHeight = startDepth;
    setTimerInterval(&DigTimer, 20000);
    setTimerInterval(&stallTimer,1500);
    while (!timerDone(&DigTimer) && continueMacro()) {
        /* WARNING : go into updateMacroCommunications and uncomment the "cotinueTheMacro= false;"*/
        updateMacroCommunications();
        //setDiggingHeight(newDrumHeight);
        
        if(DrumLimitSwitch != DrumLS_ACTIVE)
            resetTimer(&stallTimer);
        if(timerDone(&stallTimer)){
            newDrumHeight +=20;
            startDrumDig();
        }
        if (timerDone(&DrumIncTimer)) {
            newDrumHeight -= 10;
            
            if(!setDiggingHeight(newDrumHeight))
            {
                setDiggingHeight(THE_RIGHT_HEIGHT);
                return 0xFFFF;
            }
        }


    }
    //stopDrumDig();
    //    setDrumTravelMode();
    return newDrumHeight;
}

void diggingMacro2(bool b) {


}

void setDrumTravelMode() {
    delay(500);
    setMotorVel(&DrumMotor, FIND_ZERO_SPEED);
    while (DrumLimitSwitch != DrumLS_ACTIVE && continueMacro()) {
        updateMacroCommunications();
        setMotorVel(&DrumMotor, FIND_ZERO_SPEED);
        delay(500);
    }
    setMotorVel(&DrumMotor, 0);
}

bool testDrumCapacityMacro() {
    setDiggingHeight(THE_RIGHT_HEIGHT);
    /* get the capacity of the drum */
    return (getDrumWeight() >= DRUM_WEIGHT_FULL);
}
int drumHeight;

void filterMaterials(int displaceDistance) {
    drumHeight = getDrumheight();
    if (drumHeight < THE_RIGHT_HEIGHT)
        setDiggingHeight(drumHeight + 50);
    setDrumTravelMode();
    setDiggingHeight(THE_RIGHT_HEIGHT);
    driveDistance(displaceDistance, -2000);
    //driveSeperatDistances(displaceDistance,displaceDistance, -2000, -2000);

    //setDiggingHeight(THE_RIGHT_HEIGHT);
    startDrumDig();
    setTimerInterval(&filterTimer, 5000);
    while (!timerDone(&filterTimer) && continueMacro()) {
        updateMacroCommunications();
    }
    //stopDrumDig();
    //delay(500); // wait for the motor to spin down
    //setDrumTravelMode();
    //driveSeperatDistances(-displaceDistance,-displaceDistance, 1000, 1000);
    driveDistance(-displaceDistance-5, 2000);


}
//this function should be called repeatedly to assume normal operation
int currentHeight;
int newVel;
int lastHeight;
timers_t RequestTimer;

bool setDiggingHeight(unsigned int _height) {
    setTimerInterval(&RequestTimer, 300);
    currentHeight = getDrumheight();
    newVel = -(int) initDiggingPID(_height, currentHeight);

    while (!HeightTolerance(_height, currentHeight) && continueMacro()) {

        newVel = -(int) initDiggingPID(_height, currentHeight);
        setMotorVel(&ArmMotor, newVel / 2);
        /* We need a different amount of current for the different directions*/
        if (_height > currentHeight) {
            clearIntegral(&ActuatorPID);
            setMotorCurrent(&ArmMotor, 3000 * (abs(newVel) / newVel));
        } else {
            setMotorVel(&ArmMotor, 100);
            setMotorCurrent(&ArmMotor, 1000 * (abs(newVel) / newVel));

        }
        lastHeight = currentHeight;
        currentHeight = getDrumheight();
        updateMacroCommunications();
    }
    setMotorVel(&ArmMotor, 0);
    setMotorCurrent(&ArmMotor, 0);
    return true;
}
timers_t resubmitTimer, timeout;

int16_t getDrumheight() {
    //if(resubmitTimer.timerInterval != 10)
    setTimerInterval(&resubmitTimer, 58);
    setTimerInterval(&timeout, 100);
    requestMotorData(&ArmMotor, ANALOG_0_REQUESTED);
    ArmMotor.FreashAnalog = false;
    while (ArmMotor.FreashAnalog == false && !timerDone(&timeout)) {
        if (timerDone(&resubmitTimer)) {
            requestMotorData(&ArmMotor, ANALOG_0_REQUESTED);

        }
        //}else{
        //    ArmMotor.FreashAnalog = false;
    }
    return ArmMotor.Analog0;
}



#define Tolerance 10

bool HeightTolerance(int target, int actualValue) {
    if (abs(target - actualValue) <= Tolerance) {
        return true;
    } else {
        return false;
    }
}

int getDrumWeight() {
    return getCANFastData(DRUM_DATA_INDEX);

}
