/* 
 * File:   MotorDefinitions.h
 * Author: John
 *
 * Created on May 1, 2018, 3:09 PM
 */

#ifndef MOTORDEFINITIONS_H
#define	MOTORDEFINITIONS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    Velocity = 0,
    Position,
    Current

} MotorMode;

typedef struct {
    int Pos;
    int Neg;
    int Home;
} LimitSwitch_t;

typedef struct {
    //Can Address
    char ID;
    char Status;
    char MOB;
    int MAX_RPM;
    int MAX_CURRENT;
    int16_t CURRENT;
    unsigned long ACCEL;
    bool isBrushless;
    LimitSwitch_t LimitSwitch;
    long Position;
    int16_t Analog0;
    int16_t Analog1;
    uint8_t DigitalInputs;
    uint8_t DigitalOutputs;
    bool PositionReached;
    bool FreshDigitals;
    bool FreashAnalog;
    bool FreshPosition;
    int16_t errorPresent;
} Motor_t;

//WARNING!! 
//WARNING!! 
//WARNING!!  
/*
     .-""""""-.
   .'___    ___'.
  /   O      O   \
 :           `    :
 |                |   
 :    .------.    :
  \  '        '  /
   '.          .'
     '-......-'
 * */
Motor_t RightMotor, LeftMotor, DrumMotor, ArmMotor, PlowMotor;

//#define DISABLE_MOTOR_SYSTEMS
//#define DISABLE_LEFT_MOTOR
//#define DISABLE_RIGHT_MOTOR
//#define DISABLE_ARM_MOTOR
//#define DISABLE_BUCKET_MOTOR
#define DISABLE_PLOW_MOTOR


//safety constants for motor controllers
//strictly speaking the motor controllers should
//self protect, but better safe than sorry.
#define MAXRPM		 3500 //3080 is safe value printed on motors, can probably push this up to 4000 without worry if needed.
#define ACCEL_CONST 10000000 //in rev/min^2, value should be between 100k and 10k *this value will probably have to be changed under load.
//2147483647
#define ACCEL_CONST_BRUSH 1000000 //in rev/min^2, value should be between 100k and 10k *this value will probably have to be changed under load.

//maximum current allowed through BG65x25
#define MAXCURRENTBG65 8000  //seting current limit max which is actually 4A.
#define MANUAL_MIN_SPEED 30
//from msb to lsb, LeftMotor, RightMotor, ConveyorMotor, BucketMotor
//these are the bit positions for the respective motor's error bit in MOTORSTATUS error byte
#define  LEFTMOTOR_STATUS			7
#define  RIGHTMOTOR_STATUS			6
#define  ARMMOTOR_STATUS			5
#define  BUCKETMOTOR_STATUS			4
#define  PLOWMOTOR_STATUS			3


#define CAN_PACKET_SIZE 8
#define RECEIVE_MASK 0x7ff

#define ARMMOTORID 0x7C
#define ARMMOTOR_MOB 3
#define MAXCURRENTARM 5000	//this isn't the bg65 but the 75 and the current rating should be reviewed 

#define ARM_DUMP_DIGITAL  1
#define ARM_DIG_DIGITAL   3
#define ARM_HOME_DIGITAL  2

#define RIGHTMOTORID 0x7D
#define RIGHTMOTOR_MOB 1

#define LEFTMOTORID 0x7E
#define LEFTMOTOR_MOB 0

#define DRUMMOTORID 0x7F
#define DRUMMOTOR_MOB 2
#define MAXCURRENTDRUM MAXCURRENTBG65 

#define ACTUATOR_DUMP_DIGITAL 1
#define ACTUATOR_DIG_DIGITAL  2

#define PLOWMOTORID 0x7B
#define PLOWMOTOR_MOB 4
#define MAXCURRENTPLOW 3500



#define Motor_Buffer_Size			4


#define COMMSTIMEOUT 100 //ms, longest delay I have seen between message and reply is 68ms
#define COMMSCHECKTIME 1 //ms, how long between checking comms during timeout period

#define  CLEAR_ERRORS               0x3000, 0x00, 0x01
#define  SET_MOTOR_BRUSHED          0x3900, 0x00, 0x00
#define  SET_MOTOR_BRUSHLESS        0x3900, 0x00, 0x01
#define  SET_POLES                  0x3910, 0x00, 10
#define  SET_POLES_BG75             0x3910, 0x00, 8

#define  MOTOR_POLARITY             0x3911, 0x00, 0x02
#define  MOTOR_POLARITY_bg75        0x3911, 0x00, 0x00
#define  SET_FEEDBACK_ENCODER       0x3350, 0x00, 2410 //2410 for encoder
#define  SET_FEEDBACK_EMF           0x3350, 0x00, 274 //2410 for encoder
#define  SET_FEEDBACK_AN0           0x3350, 0x00, 256  //Feedback through the analogue input 0  (DC-motors only)
#define  SET_FEEDBACK_HALL          0x3350, 0x00, 2378 //2378 for hall
#define  SET_SVEL_FEEDBACK_ENCODER  0x3550, 0x00, 2410 //2410 for encoder
#define  SET_SVEL_FEEDBACK_HALL     0x3550, 0x00, 2378 //2378 for hall
#define  SET_FEEDBACK_RESOLUTION    0x3962, 0x00, 4000
#define  SET_FEEDBACK_RESOLUTION_BG75 0x3962, 0x00, 4096
#define  CURRENT_LIMIT_POS          0x3221, 0//, MAXCURRENTBG65
#define  CURRENT_LIMIT_NEG          0x3223, 0//, MAXCURRENTBG65
#define  VELOCITY_LIMIT_POS         0x3321, 0x00//, MAXRPM
#define  VELOCITY_LIMIT_NEG         0x3323, 0x00//, MAXRPM
#define  POWER_ENABLE               0x3004, 0x00, 0x01
#define  POWER_DISABLE              0x3004, 0x00, 0x00
#define  MOTOR_VOLTAGE				0x3110, 0x00, 0x00
#define  MOTOR_TEMPERATURE			0x3114, 0x00, 0x00

//*******limit switch Configs***********
#define  POSITIVE_LIMIT_SWITCH		0x3055, 0x00//, 0x130 //activates digital input 0 as positive limit switch high active
#define  NEGATIVE_LIMIT_SWITCH		0x3056, 0x00//, 0x132 //activates digital input 2 as negative limit switch high active
#define  HOME_LIMIT_SWITCH          0x3057, 0x00//, 0x131 //activates digital input 1 as negative limit switch high active

#define  LIM_D0H					0x130		//digital input 0 as limit switch active high
#define  LIM_D1H					0x131		//digital input 1 as limit switch active high
#define  LIM_D2H					0x132		//digital input 2 as limit switch active high
#define  LIM_D0L					-0x130		//digital input 0 as limit switch active low
#define  LIM_D1L					-0x131		//digital input 1 as limit switch active low
#define  LIM_D2L					-0x132		//digital input 2 as limit switch active low
//*******End of Limit switch Configs*****

#define  MODE_VEL                   0x3003, 0x00, 0x3
#define  DESIRED_VELOCITY           0x3300, 0x0
#define  ZERO_VELOCITY              0x3300, 0x0, 0x00

#define  MODE_POS                   0x3003, 0x00, 0x7
#define  POSITION_WINDOW            0x373A, 0x00, 1000
#define  POSITION_ERROR_WINDOW      0x3732, 0x00, 0xFFFFFFFF
#define  RESET_POSITION             0x3762, 0x00, 0x00


#define  MODE_CURR                  0x3003, 0x00, 0x2
#define  DESIRED_Current            0x3200, 0x0


#define  MOTOR_COUNTS               0x3791, 0x00

#define  VEL_ACC                    0x3380, 0x00//, ACCEL_CONST
#define  VEL_DEC                    0x3381, 0x00//, ACCEL_CONST


//Digital Outputs - Port0                           --3150.00h 
#define  MOTOR_DIGITAL_OUTPUT_SET   0x3150, 0x00

#define MOTOR_CONTINUE_OPERATION    0x3000, 0x00, 0x04

//Actual Target Position                            --3760.00h 
#define MOTOR_TARGET_POSITION       0x3760, 0x00


//REGISTERS FOR REQUESTING DATA FROM MOTOR
//SSI Absolute Encoder - Actual Position        -- 397A.00h
#define  SSI_ENCODER_POSITION_REQUEST   0x3762, 0x0, 0x00
#define  SSI_ENCODER_POSITION_RESET     0x3762, 0x00, 0x00
//MOTOR_ENC_ActualPosition                      -- 396A.00h 
#define  MOTOR_ENCODER_POSITION_REQUEST 0x396A, 0x00, 0x00
#define  MOTOR_ENCODER_POSITION_RESET   0x396A, 0x00, 0x00
//MOTOR_HALL_ActualPosition                     -- 394A.00h
#define  MOTOR_HALL_POSITION_REQUEST    0x394A, 0x00, 0x00
//Analogue Input 0                              -- 3100.00h
#define  MOTOR_ANALOG_0_INPUT_REQUEST   0x3100, 0x00, 0x00
//Analogue Input 1                              -- 3101.00h
#define  MOTOR_ANALOG_1_INPUT_REQUEST   0x3101, 0x00, 0x00    
//Digital Inputs                                -- 3120.00h 
#define  MOTOR_DIGITAL_INPUT_REQUEST    0x3120, 0x00, 0x00
//Motor Request Reached
#define  MOTOR_POSITION_REACHED_REQUEST 0x3002, 0x00, 0x00
//Motor Request Error
#define MOTOR_ERROR_REQUEST             0x3001, 0x00, 0x00
//Motor Current Usage
#define MOTOR_CURRENT_USAGE             0x3262, 0x00, 0x00


//DEFINES FOR HOW TO SORT DATA RETURNING FROM THE CAN BUS
#define NO_DATA_REQUESTED               0
#define ENCODER_POSITION_REQUESTED      1
#define HALL_POSITION_REQUESTED         2
#define SSI_ENCODER_POSITION_REQUESTED  3
#define ANALOG_0_REQUESTED              4
#define ANALOG_1_REQUESTED              5
#define DIGITAL_INPUT_REQUESTED         6
#define POSITION_REACHED_REQUESTED      7
#define ERROR_REQUESTED                 8
#define CURRENT_USAGE                   9


//motor status tracking
int MOTORSTATUS; //assume all motors are innocent until proven guilty


#endif	/* MOTORDEFINITIONS_H */

