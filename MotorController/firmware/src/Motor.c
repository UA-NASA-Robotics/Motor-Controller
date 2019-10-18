#include "Motor.h"
#include "MotorDefinitions.h"
#include "CAN.h"

void initMotors()
{
	//WARNING!!! - Why is limit switch have 3 ints and we are passing only 2?
	//WARNING!!! - Why is limit switch have 3 ints and we are passing only 2?
	//WARNING!!! - Why is limit switch have 3 ints and we are passing only 2?
	#ifndef DISABLE_LEFT_MOTOR

	//LeftMotor = (Motor_t){LEFTMOTORID,LEFTMOTOR_STATUS,LEFTMOTOR_MOB,MAXRPM,MAXCURRENTBG65,ACCEL_CONST,TRUE,(LimitSwitch_t){0,0},LeftMotor.Motor_Buffer};
	InitMotor(&LeftMotor, LEFTMOTORID, LEFTMOTOR_STATUS, LEFTMOTOR_MOB, MAXRPM, MAXCURRENTBG65,(LimitSwitch_t){0,0}, true);
	setMotorControlMode(&LeftMotor, Velocity,0);
	setMotorVel(&LeftMotor, 0);
    
	#endif /*DISABLE_LEFT_MOTOR*/
	
	#ifndef DISABLE_RIGHT_MOTOR
	InitMotor(&RightMotor, RIGHTMOTORID, RIGHTMOTOR_STATUS, RIGHTMOTOR_MOB, MAXRPM, MAXCURRENTBG65,(LimitSwitch_t){0,0},true);
	setMotorControlMode(&RightMotor, Velocity,0);
	setMotorVel(&RightMotor, 0);
	#endif /*DISABLE_ARM_MOTOR*/
	
	#ifndef DISABLE_ARM_MOTOR
	InitMotor(&ArmMotor, ARMMOTORID, ARMMOTOR_STATUS, ARMMOTOR_MOB, MAXRPM, MAXCURRENTARM,(LimitSwitch_t){LIM_D0H,LIM_D1H},false);
	setMotorControlMode(&ArmMotor, Current,0);
    setMotorVel(&ArmMotor, 0);
	setMotorCurrent(&ArmMotor,0);
    
    
    //setMotorCurrent(&ArmMotor, 2500);
	#endif /*DISABLE_LEFT_MOTOR*/
	
	#ifndef DISABLE_BUCKET_MOTOR
	InitMotor(&DrumMotor, DRUMMOTORID, BUCKETMOTOR_STATUS, DRUMMOTOR_MOB, MAXRPM, MAXCURRENTDRUM,(LimitSwitch_t){0,0},true);
    
	setMotorControlMode(&DrumMotor, Velocity,0);
    setMotorDeseleration(&DrumMotor);
	setMotorVel(&DrumMotor, 0);
	#endif /*DISABLE_LEFT_MOTOR*/
	
	#ifndef DISABLE_PLOW_MOTOR
	InitMotor(&PlowMotor, PLOWMOTORID, PLOWMOTOR_STATUS, PLOWMOTOR_MOB, MAXRPM, MAXCURRENTPLOW,(LimitSwitch_t){0,0},false);
	setMotorControlMode(&PlowMotor, Velocity,0);
	setMotorVel(&PlowMotor, 0);
	#endif /*DISABLE_LEFT_MOTOR*/
	
}
void MotorsAllStop()
{
	
	#ifndef DISABLE_MOTOR_SYSTEMS
	
	setMotorVel(&RightMotor, 0);
	setMotorVel(&LeftMotor, 0);

	#ifndef DISABLE_BUCKET_MOTOR
	setMotorVel(&DrumMotor,0);
	#endif
	
	#ifndef DISABLE_ARM_MOTOR
    setMotorVel(&ArmMotor, 0);
    setMotorCurrent(&ArmMotor, 0);
	//setMotorVel(&ArmMotor, 0);
	#endif
	
	#endif
}
void setMotor_Vel(int leftSpeed,int rightSpeed)
{
    setMotorVel(&RightMotor, rightSpeed);

	setMotorVel(&LeftMotor, -leftSpeed);
    
}

void requestMotorData(Motor_t * motor, int dataRequested)
{
    switch(dataRequested)
    {
        case ENCODER_POSITION_REQUESTED:
            requestMotorPacketWithResponse(motor->ID,MOTOR_ENCODER_POSITION_REQUEST     ,HALL_POSITION_REQUESTED);             
            break;
        case HALL_POSITION_REQUESTED:        
            requestMotorPacketWithResponse(motor->ID,MOTOR_HALL_POSITION_REQUEST        ,SSI_ENCODER_POSITION_REQUESTED);   
            break;
        case SSI_ENCODER_POSITION_REQUESTED:  
            requestMotorPacketWithResponse(motor->ID,SSI_ENCODER_POSITION_REQUEST       ,ENCODER_POSITION_REQUESTED);          
            break;   
        case ANALOG_0_REQUESTED:
            requestMotorPacketWithResponse(motor->ID,MOTOR_ANALOG_0_INPUT_REQUEST       ,ANALOG_0_REQUESTED);
            motor->FreashAnalog = false;
            break;
        case ANALOG_1_REQUESTED:
            requestMotorPacketWithResponse(motor->ID,MOTOR_ANALOG_1_INPUT_REQUEST       ,ANALOG_1_REQUESTED);  
            motor->FreashAnalog = false;
            break;
        case DIGITAL_INPUT_REQUESTED:
            requestMotorPacketWithResponse(motor->ID,MOTOR_DIGITAL_INPUT_REQUEST        ,DIGITAL_INPUT_REQUESTED);    
            break;
        case POSITION_REACHED_REQUESTED:            
            requestMotorPacketWithResponse(motor->ID,MOTOR_POSITION_REACHED_REQUEST     ,POSITION_REACHED_REQUESTED);        
            break;
        case ERROR_REQUESTED:
            requestMotorPacketWithResponse(motor->ID,MOTOR_ERROR_REQUEST     ,ERROR_REQUESTED); 
            break;
        case CURRENT_USAGE:
            requestMotorPacketWithResponse(motor->ID,MOTOR_CURRENT_USAGE     ,CURRENT_USAGE); 
            break;
            
    }
}