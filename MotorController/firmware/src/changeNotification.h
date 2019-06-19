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

#define INPUT_INTURRPT_PIN_0 PORTBbits.RB11         // pin24 - IO4   - From Pin 28 on Gyro
#define INPUT_INTURRPT_PIN_1 PORTBbits.RB13         // pin28 - IO2   - From pin 23 on Gyro
#define INPUT_INTURRPT_PIN_2 PORTDbits.RD9          // pin43 - IO8   - From Pin 24 on Navi  //NOT listening
#define INPUT_INTURRPT_PIN_3 PORTDbits.RD11         // pin45 - IO6   - From Pin 22 on Navi

#define OUTPUT_INTURRPT_PIN_0 PORTBbits.RB10        // pin23 - IO3   - To Pin 27 on Gyro 
#define OUTPUT_INTURRPT_PIN_1 PORTBbits.RB12        // pin27 - IO1   - To Pin 22 on Gyro
#define OUTPUT_INTURRPT_PIN_2 LATDbits.LATD10        // pin44 - IO7   - To Pin 23 on Navi //NOT TOGGLEING
#define OUTPUT_INTURRPT_PIN_3 PORTDbits.RD0         // pin46 - IO5   - To Pin 21 on Navi

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

