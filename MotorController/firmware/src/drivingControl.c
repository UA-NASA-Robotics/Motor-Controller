#include <xc.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Definitions.h"
#include "drivingControl.h"
#include "DataPublishing.h"
#include "motorHandler.h"
#include "MotorDefinitions.h"
#include "Definitions.h"
#include "Timers.h"
#include "Motor.h"
#include "macroManagement.h"
#include "changeNotification.h"
#include "PID.h"
#include <math.h>
#include "CAN_Handler/GlobalCAN_IDs.h"


#define COUNTS_PER_CENTI 2910//3108//2797//3636

typedef enum {
    Initialization = 0,
    Drive
} DriveMacro_t;
DriveMacro_t DriveMacroState = Initialization;
timers_t driveTimer;
bool leftReached = false;
bool rightReached = false;

PID_Struct_t headingPID;
PID_Struct_t DrivePID;
point_t myLoc;
short myHeading;
point_t destination;
void getNewPIDvals(point_t _destPoint, double *_drivePID, double *_headingPID);

void getNewPIDvals(point_t _destPoint, double *_drivePID, double *_headingPID) {

    myLoc.x = getCANFastData(FT_GLOBAL, getGBL_Data(MASTER_CONTROLLER, DATA_0));
    myLoc.y = getCANFastData(FT_GLOBAL, getGBL_Data(MASTER_CONTROLLER, DATA_1));

    double num = myLoc.x - _destPoint.x;
    double den = myLoc.y - _destPoint.y;
    double heading = 0;
    heading = 270.0 - (atan2((double) (_destPoint.x - myLoc.x), (double) (_destPoint.y - myLoc.y)) * RAD_TO_DEGREE + 180.0);
    if (heading < 0) heading += 360;

    *_headingPID = heading;
    *_drivePID = sqrt((double) (_destPoint.x * myLoc.x) + (double) (_destPoint.y * myLoc.y));
}
int Lspeed, Rspeed, Hval, Dval;
double heading = 0;
point_t _destPoint;
point_t pointFrom;
int distance;
timers_t transTime;
int distanceBuf[10], DistHead = 0;
int val2;
int x,y;
bool drive2Point(int val) {
    double headingVal, DriveVal;
     x = ((val >> 8)&0xFF);
    y = (val & 0xFF);
    val2 = val;
    _destPoint = (point_t){((double) ((val >> 8)&0xFF)), (double) (val & 0xFF)};
   
    switch (DriveMacroState) {

        case Initialization:
            setTimerInterval(&transTime, 50);
            //
            //            getNewPIDvals(_destPoint, &DriveVal, &headingVal);
            //            setMotorControlMode(&LeftMotor, Velocity, 0);
            //            setMotorControlMode(&RightMotor, Velocity, 0);
            INIT_PID(&headingPID, 0, 8, 0, 0);
            // The goal for the drive is to have the distance between the current
            // location and the destination to be zero
            INIT_PID(&DrivePID, 0, 22, 0, 0);
            setPID_MIN(&DrivePID, 400);
            DriveMacroState = Drive;
            break;
        case Drive:

            // Parsing Received information
            ReceiveDataCAN(FT_GLOBAL);
            // getting the information form global received array
            myHeading = getCANFastData(FT_GLOBAL, getGBL_Data(POZYX, DATA_3));
            // making the current point to be in 10*cm scale
            pointFrom = (point_t){(double) (getCANFastData(FT_GLOBAL, getGBL_Data(POZYX, DATA_0))) / 100.0,
                ((double) getCANFastData(FT_GLOBAL, getGBL_Data(POZYX, DATA_1)) / 100.0)};
            // this is the current heading of the robot
            DriveVal = sqrt(pow(_destPoint.x - pointFrom.x, 2) + pow((_destPoint.y - pointFrom.y), 2));
            if (DriveVal < 2.5) {
                setMotorControlMode(&LeftMotor, Velocity, 0);
                setMotorControlMode(&RightMotor, Velocity, 0);
                setMotor_Vel(0, 0);
                DriveMacroState = Initialization;
                return true;
            }


            Dval = updateOutput(&DrivePID, (float) DriveVal);
            if (timerDone(&transTime)) {
                // Get the heading from current location to the destination point in 0->360 that matches the scale everywhere else
                heading = 270.0 - (atan2((double) (_destPoint.x - pointFrom.x), (double) (_destPoint.y - pointFrom.y)) * RAD_TO_DEGREE + 180.0);
                if (heading < 0) heading += 360;

                int phi = abs(myHeading - (int) heading); // This is either the distance or 360 - distance
                distance = phi > 180 ? (360 - phi) : (myHeading - (int) heading);
                bufPutValCustom(distanceBuf, &DistHead, distance, sizeof (distanceBuf));
                distance = getBufAVG(distanceBuf, sizeof (distanceBuf));
                Hval = updateOutput(&headingPID, (float) distance);


                Lspeed = Dval * (-1) - Hval;
                Rspeed = Dval * (-1) + Hval;

                setMotor_Vel(Lspeed, Rspeed);
            }
            break;
    }
    return false;
}

void resetDriveStates() {
    DriveMacroState = Initialization;
}

bool driveDist(int _distance) {
    if (driveDistance(_distance, 1000)) {
        resetDriveStates();
        return true;
    }
    return false;
}

//Send a number of centimeters for each tred to travel, let 'em go

bool driveDistance(int _distance, int _speed) {
    return driveSeperatDistances(_distance, _distance, _speed, _speed);
}

bool driveSeperatDistances(int L_distance, int R_distance, int L_speed, int R_speed) {
    switch (DriveMacroState) {
        case Initialization:
#ifdef REVERSE_DRIVE_DIRECTION
            L_distance = (-1) * L_distance;
            R_distance = (-1) * R_distance;
            L_speed = (-1) * L_speed;
            R_speed = (-1) * R_speed;
#endif

            publishDataIndex(DEVICE_MACRO);
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
            DriveMacroState = Drive;
            break;
        case Drive:
            updateMacroCommunications();
            if (timerDone(&driveTimer)) {
                publishDataIndex(DEVICE_MACRO);
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
            if ((!getMotorPosReached(&LeftMotor) || !getMotorPosReached(&RightMotor)) && (notFaulted) && continueMacro()) {
                setMotorControlMode(&LeftMotor, Velocity, 0);
                setMotorControlMode(&RightMotor, Velocity, 0);

                setMotor_Vel(0, 0);
                DriveMacroState = Initialization;
                return true;
            } else {
                return false;
            }
            break;
    }


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