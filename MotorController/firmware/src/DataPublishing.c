#include <stddef.h>
#include "DataPublishing.h"
#include "CANFastTransfer.h"
#include "Definitions.h"
#include "Timers.h"

#define isINITbit   0
#define DATA_ELEMENTS_COUNT 8
timers_t dataPeriodTimer[8];
int (*dataRetrievalFunc[8])();

void initGlobalData(GLOBL_dataElement_t _index,int (*getFuncPointer)(void), unsigned long _interval)
{
    setTimerInterval(&dataPeriodTimer[_index],_interval);
    dataRetrievalFunc[_index] = getFuncPointer;
    
}

    void publishData()
    {
        int i;
        // Loop through all the elements that we could potencially send
        for(i = 0; i < DATA_ELEMENTS_COUNT; i++)
        {
            // if an data element hasn't been initialized then the pointer to the dat retrieval function will be NULL
            if(dataRetrievalFunc[i] != NULL)
            {
               // Make sure the interval that we want to send the data out on has ellapsed
               if(timerDone(&dataPeriodTimer[i]))
               {
                   //Send the data on the can bus
               }
            }
        }
    }