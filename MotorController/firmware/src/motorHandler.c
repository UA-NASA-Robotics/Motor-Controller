#include "motorHandler.h"
#include "Motor.h"

void setMotorEncoderResolution(Motor_t *motor, uint16_t res);
void setMotorRamp(Motor_t *motor);

void InitMotor(Motor_t* motor, char address, char statusBit, char mob, int maxRPM, int maxCurrent, LimitSwitch_t limitSwitch, bool brushless) {
    motor->ID = address;
    motor->Status = statusBit;
    motor->MOB = mob;
    motor->MAX_RPM = maxRPM;
    motor->MAX_CURRENT = maxCurrent;
    motor->LimitSwitch = limitSwitch;
    motor->isBrushless = brushless;
    motor->Position = 0;
    motor->DigitalInputs = 0xFF;
    motor->Analog0 = 0;
    motor->Analog1 = 0;
    motor->PositionReached = false;
    motor->FreshDigitals = false;
    motor->FreshPosition = false;

    sendMotorPacket(address, CLEAR_ERRORS);
    sendMotorPacket(address, POWER_DISABLE);
    if (brushless) {
        sendMotorPacket(address, SET_FEEDBACK_ENCODER);
    } else {
        sendMotorPacket(address, SET_FEEDBACK_EMF);
    }

    //sendMotorPacket(address,POSITION_WINDOW);
    sendMotorPacket(address, SET_SVEL_FEEDBACK_ENCODER);
    sendMotorPacket(address, SET_FEEDBACK_RESOLUTION);
    if (brushless) {
        sendMotorPacket(address, SET_MOTOR_BRUSHLESS);
    } else {
        sendMotorPacket(address, SET_MOTOR_BRUSHED);
    }
    sendMotorPacket(address, POSITION_ERROR_WINDOW);
    sendMotorPacket(address, SSI_ENCODER_POSITION_RESET);
    sendMotorPacket(address, SET_POLES);
    // Limit Switches
    sendMotorPacket(address, POSITIVE_LIMIT_SWITCH, motor->LimitSwitch.Pos);
    sendMotorPacket(address, NEGATIVE_LIMIT_SWITCH, motor->LimitSwitch.Neg);
    // Max Currents
    sendMotorPacket(address, CURRENT_LIMIT_POS, maxCurrent);
    sendMotorPacket(address, CURRENT_LIMIT_NEG, maxCurrent);
    // Max RPMs
    sendMotorPacket(address, VELOCITY_LIMIT_POS, maxRPM);
    sendMotorPacket(address, VELOCITY_LIMIT_NEG, maxRPM);
    // Direction of rotation
    sendMotorPacket(address, MOTOR_POLARITY);
    // Enable Motor
    sendMotorPacket(address, POWER_ENABLE);
    sendMotorPacketWithResponse(address, MOTOR_ANALOG_0_INPUT_REQUEST, ANALOG_0_REQUESTED);
    sendMotorPacketWithResponse(address, MOTOR_DIGITAL_INPUT_REQUEST, DIGITAL_INPUT_REQUESTED);
}

void InitMotor_BG75(Motor_t* motor, char address, char statusBit, char mob, int maxRPM, int maxCurrent, LimitSwitch_t limitSwitch) {

    motor->ID = address;
    motor->Status = statusBit;
    motor->MOB = mob;
    motor->MAX_RPM = maxRPM;
    motor->MAX_CURRENT = maxCurrent;
    motor->LimitSwitch = limitSwitch;
    motor->Position = 0;
    motor->DigitalInputs = 0xFF;
    motor->Analog0 = 0;
    motor->Analog1 = 0;
    motor->PositionReached = false;
    motor->FreshDigitals = false;
    motor->FreshPosition = false;

    //wipe errors. This first attempt at communication also verifies the controller is in the system.
    sendMotorPacket(address, CLEAR_ERRORS);
    //SendandVerify(PowerDisable,  &Motor->Motor_Buffer, Motor->Status);

    sendMotorPacket(address, POSITION_ERROR_WINDOW);
    //configure motor, number of magnet poles and motor type.
    sendMotorPacket(address, SET_MOTOR_BRUSHLESS);
    sendMotorPacket(address, SET_POLES_BG75);
    //disable controller so some commands actually work (Feed back commands)
    // Limit Switches
    sendMotorPacket(address, POSITIVE_LIMIT_SWITCH, motor->LimitSwitch.Pos);
    sendMotorPacket(address, NEGATIVE_LIMIT_SWITCH, motor->LimitSwitch.Neg);
    sendMotorPacket(address, HOME_LIMIT_SWITCH, motor->LimitSwitch.Home);
    // Max Currents
    sendMotorPacket(address, CURRENT_LIMIT_POS, maxCurrent);
    sendMotorPacket(address, CURRENT_LIMIT_NEG, maxCurrent);
    // Max RPMs
    sendMotorPacket(address, VELOCITY_LIMIT_POS, maxRPM);
    sendMotorPacket(address, VELOCITY_LIMIT_NEG, maxRPM);
    //actually critical to get right if you are using encoders
    sendMotorPacket(address, MOTOR_POLARITY_bg75);
    //set resolution of encoder (500 count encoder * 4 for how the controllers work)
    sendMotorPacket(address, SET_FEEDBACK_ENCODER);
    //SendandVerify(SetSVELFeedBack,  &Motor->Motor_Buffer, Motor->Status);
    sendMotorPacket(address, SET_FEEDBACK_RESOLUTION_BG75);
    //re-enable motor controller.
    sendMotorPacket(address, POWER_ENABLE);
}


//*******************************************************
//------------------------Setters------------------------
//*******************************************************

void clearMotorErrors(Motor_t * motor) {
    sendMotorPacket(motor->ID, CLEAR_ERRORS);
}

void setMotorControlMode(Motor_t *motor, unsigned char mode, int velocity) {
    switch (mode) {
        case Velocity:
        {
            sendMotorPacket(motor->ID, MODE_VEL);
            // Setting acceleration and deceleration values
            sendMotorPacket(motor->ID, VEL_ACC, (motor->isBrushless ? ACCEL_CONST : ACCEL_CONST_BRUSH));
            sendMotorPacket(motor->ID, VEL_DEC, (motor->isBrushless ? ACCEL_CONST : ACCEL_CONST_BRUSH));
            sendMotorPacket(motor->ID, DESIRED_VELOCITY, velocity);
            break;
        }
        case Position:
        {
            sendMotorPacket(motor->ID, MODE_POS);
            sendMotorPacket(motor->ID, POSITION_WINDOW);
            sendMotorPacket(motor->ID, DESIRED_VELOCITY, velocity);
            sendMotorPacket(motor->ID, RESET_POSITION);
            break;
        }
        case Current:
        {
            sendMotorPacket(motor->ID, MODE_CURR);
            sendMotorPacket(motor->ID, POSITION_WINDOW);
            sendMotorPacket(motor->ID, DESIRED_Current, velocity);
            break;

        }

    }
}

void setMotorDeseleration(Motor_t *motor)
{
  //  sendMotorPacket(motor->ID, CLEAR_ERRORS);
    //sendMotorPacket(motor->ID, POWER_DISABLE);
    sendMotorPacket(motor->ID,VEL_DEC, 200000);
    sendMotorPacket(motor->ID,VEL_ACC, 200000);
//    sendMotorPacket(motor->ID,0x334c,0x0, 1);   //ramp gen
//    sendMotorPacket(motor->ID,0x3343,0x0,1000); //Delt t
//    sendMotorPacket(motor->ID,0x3342,0x0,1000); //Delt I
    //sendMotorPacket(motor->ID,VEL_DEC, 214);
    //sendMotorPacket(motor->ID, POWER_ENABLE);
}
void setMotorEncoderResolution(Motor_t *motor, uint16_t res)
{
    sendMotorPacket(motor->ID, CLEAR_ERRORS);
    sendMotorPacket(motor->ID, POWER_DISABLE);
    sendMotorPacket(motor->ID, SET_SVEL_FEEDBACK_ENCODER);
    sendMotorPacket(motor->ID, SET_FEEDBACK_RESOLUTION);
    sendMotorPacket(motor->ID, POWER_ENABLE);

}
//void setMotorPos(Motor_t *motor, int pos)
//{
//
//
//}

void setMotorVel(Motor_t* motor, int Vel) {
    if (Vel > motor->MAX_RPM) {
        Vel = motor->MAX_RPM;
    }
    if (Vel < -(motor->MAX_RPM)) {
        Vel = -(motor->MAX_RPM);
    }
    sendMotorPacket(motor->ID, DESIRED_VELOCITY, Vel);

}
void setMotorCurrent(Motor_t* motor, int Curr) {
    if (Curr > motor->MAX_CURRENT) {
        Curr = motor->MAX_CURRENT;
    }
    if (Curr < -(motor->MAX_CURRENT)) {
        Curr = -(motor->MAX_CURRENT);
    }
    //sendMotorPacket(motor->ID, 0x3000,0, 4);
    sendMotorPacket(motor->ID, DESIRED_Current, Curr);

}
void resetMotorCounts(Motor_t * motor) {
    sendMotorPacket(motor->ID, RESET_POSITION);
}

void setMotorTargetPosition(Motor_t * motor, long counts) {
    sendMotorPacket(motor->ID, MOTOR_TARGET_POSITION, counts);
}

void setMotorCounts(Motor_t* motor, long counts) {
    sendMotorPacket(motor->ID, MOTOR_COUNTS, counts);
}

void SetMotorLimit(Motor_t *motor) {
    sendMotorPacket(motor->ID, POSITIVE_LIMIT_SWITCH, motor->LimitSwitch.Pos); //activates digital input 0 as positive limit switch high active
    sendMotorPacket(motor->ID, POSITIVE_LIMIT_SWITCH, motor->LimitSwitch.Pos); //activates digital input 1 as negative limit switch high active

    //SDO_packet HomeLimit = {motor->ID, 0x3056, 0x00, 0x132}; //activates digital input 2 as home limit switch high active
    //SDO_packet Home_Method = {BUCKETMOTORID, 0x37B2, 0x00, 2}; //homes by turning CW to positive limit, should be what we want.
}

//Outputs range from 1->4

void setMotorDigitalOutput(Motor_t * motor, uint8_t digitalOutput, bool outputState) {
    if (outputState) motor->DigitalOutputs |= (0x01 << (digitalOutput + 1)); //Set to 1 
    else motor->DigitalOutputs &= (0xFE << (digitalOutput + 1)); //Clear

    sendMotorPacket(motor->ID, MOTOR_DIGITAL_OUTPUT_SET, motor->DigitalOutputs);
}

void storeMotorError(Motor_t * motor, int16_t err) {
    motor->errorPresent = err;
}

void storeMotorPosition(Motor_t * motor, long pos) {
    motor->FreshPosition = true;
    motor->Position = pos;
}

void storeMotorAnalog0(Motor_t * motor, int16_t analog0) {
    motor->FreashAnalog = true;
    motor->Analog0 = analog0;
}

void storeMotorAnalog1(Motor_t * motor, int16_t analog1) {
    motor->FreashAnalog = true;
    motor->Analog1 = analog1;
}

void storeMotorDigitalInputs(Motor_t * motor, uint8_t digitalIn) {
    motor->FreshDigitals = true;
    motor->DigitalInputs = digitalIn;
}

void storeMotorPositionReached(Motor_t * motor, bool positionReached) {
    motor->PositionReached = positionReached;
}
void storeMotorCurrent(Motor_t* motor, int16_t cur)
{
    motor->CURRENT = cur;
}

//*********************************************************************************
//------------------------------------<Getters>------------------------------------
//*********************************************************************************

long getMotorPosition(Motor_t * motor) {
    motor->FreshPosition = false;
    return (motor->Position);
}
//Request a digital input from 1->8 from a specific motor

bool getMotorDigitalInput(Motor_t * motor, uint8_t digitalInput) {
    motor->FreshDigitals = false;
    return ((motor->DigitalInputs) & (0x01 << (digitalInput - 1)));
}

bool getMotorPosReached(Motor_t * motor) {
    return (motor->PositionReached);
}

int16_t getMotorAnalog0(Motor_t * motor) {
    return (motor->Analog0);
}

int16_t getMotorError(Motor_t * motor) {
    return motor->errorPresent;
}
//TODO: MAKE A CAN RECEIVE FUNCTION
/*
long getMotorPos(Motor_t *motor)
{
    char temp[8];
    SDO_packet ReadPosition ={motor->ID, 0x396A, 0x00, 0x00};// {RIGHTMOTORID, 0x396A, 0x00, 0x00};

    if(ReadandVerify(ReadPosition, &motor->Motor_Buffer, motor->Status, temp))
    {
        long result = ArrayToLong(temp);
        return (result/4); //returns encoder counts
    }
    else
    {
        return 0;
    }
}
char getMotorVoltage(Motor_t *motor)
{
    char temp[8];
    SDO_packet ReadVoltage = {motor->ID, MOTOR_VOLTAGE};

    if(ReadandVerify(ReadVoltage, &motor->Motor_Buffer, motor->Status, temp))
    {
        long result = ArrayToLong(temp);
        return (result/1000 +1); //get voltage in mV from controller, add 1v to compensate for suspected diode drop to input.
    }
    else
    {
        return 0;
    }
}
char getMotorTemp(Motor_t *motor)
{
    char temp[8];
    SDO_packet ReadTemperature = {motor->ID, MOTOR_TEMPERATURE};

    if(ReadandVerify(ReadTemperature, &motor->Motor_Buffer, motor->Status, temp))
    {
        long result = ArrayToLong(temp);
        return result/10;
    }
    else
    {
        return 0;
    }
}

//bool getMotorStatus(Motor_t *motor)
//{
//
//}


char getMotorDigital(Motor_t *motor)
{
    char temp[8];
    SDO_packet ReadPort0 = {motor->ID, 0x3120, 0x00, 0x00};

    ReadandVerify(ReadPort0, &motor->Motor_Buffer, motor->Status, temp);
    long result = ArrayToLong(temp);
    return result;
}
 */