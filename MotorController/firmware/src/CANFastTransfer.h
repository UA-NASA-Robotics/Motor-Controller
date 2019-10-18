/* ************************************************************************** */
/** Descriptive File Name

  @File Name
    CANFastTransfer.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _CAN_FAST_TRANSFER_H    /* Guard against multiple inclusion */
#define _CAN_FAST_TRANSFER_H
#include <stdbool.h>
#include <stdint.h>
#include "CAN.h"
#include "bufferHandler.h"
#include "Definitions.h"
void initCANFT(void);
int ReceiveDataCAN(void); 
int * getReceiveArrayCAN(void);
void ToSendCAN(unsigned int where, unsigned int what);
void sendDataCAN( unsigned int whereToSend);
void ReceiveCANFast( my_can_packet_t *p); // interrupt callback
bool TransmitCANFast( my_can_packet_t *p); // interrupt callback
int getCANFastData(uint8_t index);
#endif /* _CAN_FAST_TRANSFER_H */

/* *****************************************************************************
 End of File
 */
