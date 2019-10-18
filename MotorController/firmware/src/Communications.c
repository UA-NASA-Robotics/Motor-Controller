#include "Communications.h"
#include "CANFastTransfer.h"

int getDrumWeight()
{
   return getCANFastData(DRUM_DATA_INDEX);
    
}
