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
#include "CANbufferHandler.h"
#include "Definitions.h"



typedef enum {
    FT_LOCAL = 0,
    FT_GLOBAL
} FT_type_t;
#define GLOBAL_SYSTEM_DATA_SIZE 5
int GlobalAddressInturpret(int index);

void initCANFT(void);
int ReceiveDataCAN(FT_type_t _t);
short * getReceiveArrayCAN(void);
void ToSendCAN(unsigned int where, unsigned int what);
void sendDataCAN(unsigned int whereToSend);
void ReceiveCANFast(my_can_packet_t *p, FT_type_t _t); // interrupt callback
bool TransmitCANFast(my_can_packet_t *p); // interrupt callback
int getCANFastData(FT_type_t _t, uint8_t index);
void clearCANFastDataValue(int index);
void clearCANFastDataValueRange(int startIndex,int end);
bool getNewDataFlagStatus(FT_type_t _t, int index);
#endif /* _CAN_FAST_TRANSFER_H */

/* *****************************************************************************
 End of File
 */
