/* 
 * File:   motorHandler.h
 * Author: John
 *
 * Created on May 1, 2018, 3:11 PM
 */

#ifndef MOTORHANDLER_H
#define	MOTORHANDLER_H

#include "MotorDefinitions.h"
#include "CAN.h"
#include <stdint.h>

void InitMotor_BG75(Motor_t* motor, char address, char statusBit,char mob, int maxRPM, int maxCurrent, LimitSwitch_t limitSwitch);
void InitMotor(Motor_t* motor, char address, char statusBit,char mob, int maxRPM, int maxCurrent, LimitSwitch_t limitSwitch, bool brushless);

void clearMotorErrorStatusReg(Motor_t* motor);

void ReEstablishComms(Motor_t* motor);
//*******************************************************
//------------------------Setters------------------------
//*******************************************************


void setMotorEncoderResolution(Motor_t *motor, uint16_t res);
void setMotorDeseleration(Motor_t *motor);

void clearMotorErrors(Motor_t * motor);
void setMotorDigitalOutput(Motor_t * motor, uint8_t digitalOutput, bool outputState);

void setMotorControlMode(Motor_t *motor, unsigned char mode, int velocity);
void SetMotorLimit(Motor_t *motor);

void setMotorPos(Motor_t *motor, int pos);
void setMotorPosNoSafetyComms(Motor_t *motor, int pos);

void setMotorVel(Motor_t *motor, int Vel);

void setMotorCurrent(Motor_t* motor, int Curr);

void setMotorCounts(Motor_t* motor, long counts);
void setMotorCountsNoSafetyComms(Motor_t *motor, long counts);

void setMotorTargetPosition(Motor_t * motor, long counts);

void storeMotorPosition(Motor_t * motor, long pos);
void storeMotorPositionReached(Motor_t * motor, bool positionReached);

void storeMotorAnalog0(Motor_t * motor, int16_t analog0);
void storeMotorAnalog1(Motor_t * motor, int16_t analog1);

void storeMotorDigitalInputs(Motor_t * motor, uint8_t digitalIn);
void storeMotorCurrent(Motor_t* motor, int16_t cur);
void storeMotorError(Motor_t * motor, int16_t err);
void resetMotorCounts(Motor_t * motor);
//*******************************************************
//------------------------Getters------------------------
//*************************************************
int16_t getMotorAnalog0(Motor_t * motor);
long getMotorPosition(Motor_t * motor);
bool getMotorDigitalInput(Motor_t * motor, uint8_t digitalInput);
long getMotorPos(Motor_t *motor);
char getMotorVoltage(Motor_t *motor);
char getMotorTemp(Motor_t *motor);
bool getMotorPosReached(Motor_t * motor);
//bool getMotorStatus(Motor_t *motor);
int getMotorAnalog(Motor_t *motor);
int16_t getMotorError(Motor_t * motor);

#endif	/* MOTORHANDLER_H */

