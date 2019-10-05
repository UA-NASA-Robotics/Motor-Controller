#include "DataPublishing.h"
#include "CANFastTransfer.h"
#include "Definitions.h"
#define isINITbit   0

void HandleDataPublish();

bool isMotorsInicialized = false;

void isDataRequested()
{
    int result = getCANFastData(10);
    if(result != 0xffff && result == 0x0101)
    {
        HandleDataPublish();
        clearCANFastData(10);
    }
}

void HandleDataPublish()
{
    ToSendCAN(0x0A,1);
    sendDataCAN(ROUTER_ADDRESS);
}

void MotorsInicialized(bool state)
{
    isMotorsInicialized = state;
    if(state)
        state = 1;
    HandleDataPublish();
}