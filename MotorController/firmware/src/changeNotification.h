/* 
 * File:   changeNotification.h
 * Author: Seth Carpenter
 *
 * Created on April 21, 2018, 8:01 PM
 */

#ifndef CHANGENOTIFICATION_H
#define	CHANGENOTIFICATION_H
//#include "boolean.h"
#include "stdbool.h"
#include <xc.h>

#define CN_PIN_COUNT 4      //the number of Change notification input pins

#define INPUT_INTURRPT_PIN_0 PORTBbits.RB11         // pin24 - IO11   - From Pin 22 on Gyro
#define INPUT_INTURRPT_PIN_1 PORTBbits.RB13         // pin28 - IO13   - From pin 23 on Gyro
#define INPUT_INTURRPT_PIN_2 PORTEbits.RE0          // pin58 - IO7    - From Pin 29 on Navi 
#define INPUT_INTURRPT_PIN_3 PORTFbits.RF1          // pin57 - IO8    - From Pin 30 on Navi

#define OUTPUT_INTURRPT_PIN_0 LATBbits.LATB10        // pin23 - IO3   - To Pin 27 on Gyro 
#define OUTPUT_INTURRPT_PIN_1 LATBbits.LATB12        // pin27 - IO1   - To Pin 22 on Gyro
#define OUTPUT_INTURRPT_PIN_2 LATEbits.LATE3         // pin63 - IO5   - To Pin 27 on Navi 
#define OUTPUT_INTURRPT_PIN_3 LATEbits.LATE2         // pin62 - IO6   - To Pin 28 on Navi

typedef struct{
    unsigned char pinId;    //this should be a number between 0 and 3 that corresponds to the input pin
    unsigned char prevState;
    bool changed;
}intPin_t;

intPin_t GyroPin1;
intPin_t GyroPin2;
intPin_t NaviPin1;
intPin_t NaviPin2;

void initChangeNotification();
bool pinState(intPin_t *pin);
void togglePinState(intPin_t *pin);
void pinChangeNotified();
void setPinState(intPin_t *pin, unsigned char state);
#endif	/* CHANGENOTIFICATION_H */

