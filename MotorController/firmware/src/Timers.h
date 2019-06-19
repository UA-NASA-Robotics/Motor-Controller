
#ifndef TIMERS_H
#define TIMERS_H
//*****************************************
//********TIMER.H file code****************
//*****************************************
#include <stdbool.h>

 typedef struct{
        unsigned long timerInterval;
        unsigned long lastMillis;
    }timers_t;
    
bool timerDone(timers_t * t);
void setTimerInterval(timers_t * t, unsigned long interval);
void resetTimer(timers_t * t);
void globalTimerTracker( );
unsigned long millis(void);
void delay(unsigned long val);
//*****************************************
//**********    END OF H file  ************
//*****************************************

#endif  /*END OF TIMER_H*/
