#include "DataPublishing.h"
#include "CANFastTransfer.h"
#include "Definitions.h"
#define isINITbit   0



bool isMotorsInicialized = false;

void isDataRequested()
{
    int result = getCANFastData(10);
    if(result != 0xffff && result == 0x0101)
    {
        HandleDataPublish();
    }
}

void HandleDataPublish()
{
    ToSendCAN(0x00,isMotorsInicialized);
    sendDataCAN(ROUTER_ADDRESS);
}

void MotorsInicialized(bool state)
{
    isMotorsInicialized = state;
    if(state)
        state = 1;
    HandleDataPublish();
}