/* 
 * File:   Motors.h
 * Author: John
 *
 * Created on May 1, 2018, 4:00 PM
 */

#ifndef MOTOR_H
#define	MOTOR_H

#include "motorHandler.h"
#include "CAN_Handler/CAN.h"

#define getManualDriveSpeed() getCANFastData(FT_LOCAL, DRIVE_SPEED)
#define getManualArmSpeed() getCANFastData(FT_LOCAL, ARM_SPEED)
#define getManualBucketSpeed() getCANFastData(FT_LOCAL, BUCKET_SPEED)

void initMotors();
void MotorsAllStop();
void setMotor_Vel(int leftSpeed,int rightSpeed);
void requestMotorData(Motor_t * motor, int dataRequested);
void handleManualControl(uint16_t _driveMotorSpeed,uint16_t _speed1,uint16_t _speed2,uint16_t _speed3);

#endif	/* MOTOR_H */

