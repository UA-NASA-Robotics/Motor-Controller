#include <xc.h>
#include <stdbool.h>
#include "drivingControl.h"
#include "motorHandler.h"
#include "MotorDefinitions.h"
#include "Definitions.h"
#include "Timers.h"
#include "Motor.h"
#include "macroManagement.h"
#include "changeNotification.h"

#define COUNTS_PER_CENTI 2910//3108//2797//3636

timers_t driveTimer;
bool leftReached = false;
bool rightReached = false;



//Send a number of centimeters for each tred to travel, let 'em go

void driveSeperatDistances(int L_distance, int R_distance, int L_speed, int R_speed) {

#ifdef REVERSE_DRIVE_DIRECTION
    L_distance = (-1) * L_distance;
    R_distance = (-1) * R_distance;
    L_speed = (-1) * L_speed;
    R_speed = (-1) * R_speed;
#endif
    
    
    //setPinState(&GyroPin2,1);
    //Setup timer System
    setTimerInterval(&driveTimer, 50);

    //ADD THIS!!
    bool notFaulted = true;
    leftReached = false;
    rightReached = false;

    LeftMotor.PositionReached = false;
    RightMotor.PositionReached = false;

    //set position mode
    setMotorControlMode(&LeftMotor, Position, L_speed);
    setMotorControlMode(&RightMotor, Position, R_speed);

    //Motor Position Clear
    storeMotorPositionReached(&LeftMotor, false);
    storeMotorPositionReached(&RightMotor, false);

    //Clear the positioning on the motors
    sendMotorPacket(LEFTMOTORID, SSI_ENCODER_POSITION_RESET);
    sendMotorPacket(RIGHTMOTORID, SSI_ENCODER_POSITION_RESET);

    //set to drive a distance
    setMotorCounts(&LeftMotor, -L_distance * COUNTS_PER_CENTI);
    setMotorCounts(&RightMotor, R_distance * COUNTS_PER_CENTI);

    while ((!getMotorPosReached(&LeftMotor) || !getMotorPosReached(&RightMotor)) && (notFaulted) && continueMacro()) {
        updateMacroCommunications();
        if (timerDone(&driveTimer)) {
            requestMotorData(&LeftMotor, ERROR_REQUESTED);
            requestMotorData(&RightMotor, ERROR_REQUESTED);
            requestMotorData(&LeftMotor, ENCODER_POSITION_REQUESTED);
            requestMotorData(&RightMotor, ENCODER_POSITION_REQUESTED);
            requestMotorData(&LeftMotor, POSITION_REACHED_REQUESTED);
            requestMotorData(&RightMotor, POSITION_REACHED_REQUESTED);
            LED1 ^= 1;
            LED2 ^= 1;
            LED3 ^= 1;
            LED4 ^= 1;

            if (getMotorPosReached(&LeftMotor) && !leftReached) {
                setMotorControlMode(&LeftMotor, Velocity, 0);
                //setMotorVel(&LeftMotor,0);
                leftReached = true;
            }

            if (getMotorPosReached(&RightMotor) && !rightReached) {
                setMotorControlMode(&RightMotor, Velocity, 0);
                //setMotorVel(&RightMotor,0);
                rightReached = true;
            }

            if (getMotorError(&RightMotor)) {
                clearMotorErrors(&RightMotor);
                int resetCounts = R_distance * COUNTS_PER_CENTI - RightMotor.Position;
                setMotorCounts(&RightMotor, resetCounts);

            }
            if (getMotorError(&LeftMotor)) {
                clearMotorErrors(&LeftMotor);
                int resetCounts = -L_distance * COUNTS_PER_CENTI - LeftMotor.Position;
                setMotorCounts(&LeftMotor, resetCounts);
            }
        }
    }

    //setPinState(&GyroPin2,0);

    //set back to velocity mode    
    setMotorControlMode(&LeftMotor, Velocity, 0);
    setMotorControlMode(&RightMotor, Velocity, 0);

    setMotor_Vel(0, 0);
    //delay(1000);
}

//Send a number of centimeters to travel, let 'em go

void driveDistanceVariedSpeed(int distance, int speed) {
    bool changedR = false;
    bool changedL = false;
    //set position mode
    setMotorControlMode(&LeftMotor, Position, speed);
    setMotorControlMode(&RightMotor, Position, speed);

    //Motor Position Clear
    storeMotorPositionReached(&LeftMotor, false);
    storeMotorPositionReached(&RightMotor, false);

    sendMotorPacket(LEFTMOTORID, SSI_ENCODER_POSITION_RESET);
    sendMotorPacket(RIGHTMOTORID, SSI_ENCODER_POSITION_RESET);

    //set to drive a distance
    setMotorCounts(&LeftMotor, -distance * COUNTS_PER_CENTI);
    setMotorCounts(&RightMotor, distance * COUNTS_PER_CENTI);
    LED1 = 0;
    LED2 = 0;

    while ((!getMotorPosReached(&LeftMotor) || !getMotorPosReached(&RightMotor)) && continueMacro()) {
        requestMotorData(&LeftMotor, POSITION_REACHED_REQUESTED);
        requestMotorData(&RightMotor, POSITION_REACHED_REQUESTED);
        requestMotorData(&LeftMotor, SSI_ENCODER_POSITION_REQUESTED);
        requestMotorData(&RightMotor, SSI_ENCODER_POSITION_REQUESTED);
        //LED1 ^=1;
        delay(50);
        //LED2 ^=1;
        LED3 ^= 1;
        delay(50);
        LED4 ^= 1;
        if (abs(LeftMotor.Position) > 90000 && !changedL) {
            long distance_remaining = -(distance * COUNTS_PER_CENTI) - LeftMotor.Position;
            setMotorControlMode(&LeftMotor, Velocity, speed / 2);
            setMotorControlMode(&LeftMotor, Position, speed / 2);
            setMotorCounts(&LeftMotor, distance_remaining);
            changedL = true;
            LED1 = 1;
        }

        if (abs(RightMotor.Position) > 1000 && !changedR) {
            setMotorVel(&RightMotor, 3000);
            changedR = true;
            LED2 = 1;
        }

        if (getMotorPosReached(&LeftMotor)) {
            setMotorVel(&LeftMotor, 0);
        }
        if (getMotorPosReached(&RightMotor)) {
            setMotorVel(&RightMotor, 0);
        }
    }

    //set back to velocity mode    
    setMotorControlMode(&LeftMotor, Velocity, 0);
    setMotorControlMode(&RightMotor, Velocity, 0);

    setMotor_Vel(0, 0);
}