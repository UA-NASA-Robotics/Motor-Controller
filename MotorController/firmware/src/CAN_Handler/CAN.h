/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _CAN_H    /* Guard against multiple inclusion */
#define _CAN_H

#include "bufferHandler.h"
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>


#ifndef NO_DATA_REQUESTED               
#define NO_DATA_REQUESTED               0
#endif

#define DLC_VALUE       8

typedef struct
{
    uint16_t    objAddr;
    uint8_t     subAddr;
    long        data;
}SDO_PACKET_t;

#ifndef DATA_T
#define DATA_T

typedef struct {
    int data;
    bool newDataFlag;
} Data_t;

#endif

void initCANISRs(void);
void CAN_ISR_CALLBACK(void);
void messageTransmit(my_can_packet_t packetToSend);
void messageTransmitWithResponse(my_can_packet_t packetToSend, int responseType, Data_t *_callbackPtr);
bool CANmessageAvailable(void);
bool messageReceive(my_can_packet_t * packetToReceive);
void populateMotorPacket(uint8_t * messageData, SDO_PACKET_t packetToSend);

void sendMotorPacket(uint8_t motorAddress, uint16_t objectAddr, uint8_t subAddr, long data);
void sendMotorPacketInit(uint8_t motorAddress, uint16_t objectAddr, uint8_t subAddr, long data);
//void (uint8_t motorAddress, uint16_t objectAddr, uint8_t subAddr, long data, int responseType);

void requestMotorPacketWithResponse(uint8_t motorAddress, uint16_t objectAddr, uint8_t subAddr, long data, int responseType, Data_t *_callback);

#endif /* _CAN_H */

/* *****************************************************************************
 End of File
 */