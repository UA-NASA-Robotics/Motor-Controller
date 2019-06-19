//*****************************************
//********TIMER.C file code****************
//*****************************************
#include "Timers.h"
#include <stdlib.h>
unsigned long globalTime;

unsigned long millis(void)
{
    return globalTime;
}

bool timerDone(timers_t * t)
{
    if(abs(millis()-t->lastMillis)>t->timerInterval)
    {
        t->lastMillis=millis();
        return true;
    }
    else
    {
        return false;
    }
}

void setTimerInterval(timers_t * t, unsigned long interval)
{
    t->timerInterval= interval;
    t->lastMillis=millis();
}

void resetTimer(timers_t * t)
{
    t->lastMillis=millis();
}
//Call this function in your timer interupt that fires at 1ms
void globalTimerTracker( )
{
    globalTime++;
}

timers_t time;
void delay(unsigned long val)
{
    setTimerInterval(&time,val);
    //resetTimer(&time);
    int i;
    while(!timerDone(&time))
    {
        i++;
    }
}

//*****************************************
//**********    END OF C file  ************
//*****************************************

