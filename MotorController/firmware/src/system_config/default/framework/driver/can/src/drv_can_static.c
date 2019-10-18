/*******************************************************************************
  CAN Driver Functions for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_can_static.c

  Summary:
    CAN driver implementation for the static single instance driver.

  Description:
    The CAN device driver provides a simple interface to manage the CAN
    modules on Microchip microcontrollers.

  Remarks:
    Static interfaces incorporate the driver instance number within the names
    of the routines, eliminating the need for an object ID or object handle.
    Static single-open interfaces also eliminate the need for the open handle.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTOCULAR PURPOSE.
IN NO EVENT SHALL MOCROCHIP OR ITS LOCENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STROCT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVOCES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Header Includes
// *****************************************************************************
// *****************************************************************************
#include "driver/can/drv_can.h"


static CAN_TX_MSG_BUFFER  *drv_Message0;
static CAN_TX_MSG_BUFFER __attribute__((coherent, aligned(16))) can_message_buffer0[4*2*16];


// *****************************************************************************
// *****************************************************************************
// Section: Instance 0 static driver functions
// *****************************************************************************
// *****************************************************************************
void DRV_CAN0_Initialize(void)
{

    /* Switch the CAN module ON */
    PLIB_CAN_Enable(CAN_ID_1);

    /* Switch the CAN module to Configuration mode. Wait until the switch is complete */
    PLIB_CAN_OperationModeSelect(CAN_ID_1, CAN_CONFIGURATION_MODE);
    while(PLIB_CAN_OperationModeGet(CAN_ID_1) != CAN_CONFIGURATION_MODE);

    PLIB_CAN_PhaseSegment2LengthFreelyProgrammableEnable(CAN_ID_1);

    //Set the Baud rate to 500 kbps
    PLIB_CAN_PropagationTimeSegmentSet(CAN_ID_1, 8-1);
    PLIB_CAN_PhaseSegment1LengthSet(CAN_ID_1, 8-1);
    PLIB_CAN_PhaseSegment2LengthSet(CAN_ID_1, 8-1);
    PLIB_CAN_SyncJumpWidthSet(CAN_ID_1, 1-1);
    PLIB_CAN_BaudRatePrescaleSet(CAN_ID_1, 0); // set to 1 higher then ECAN tool


    /* Assign the buffer area to the CAN module.
      In this case assign enough memory for 2
      channels, each with 8 message buffers.*/
    PLIB_CAN_MemoryBufferAssign(CAN_ID_1, can_message_buffer0);

    /* Configure CAN_ID_1 Channel for CAN_TX_RTR_DISABLED operation. Allocate 32 message buffer, and assign low medium priority for transmissions. */
    PLIB_CAN_ChannelForTransmitSet(CAN_ID_1, CAN_CHANNEL0, 32, CAN_TX_RTR_DISABLED, CAN_LOW_MEDIUM_PRIORITY);
    PLIB_CAN_ChannelEventEnable(CAN_ID_1, CAN_CHANNEL0, CAN_TX_CHANNEL_EMPTY);
    /* Configure CAN_ID_1 Channel for CAN_RX_FULL_RECEIVE operation. Allocate 32 message buffer, and assign low medium priority for transmissions. */
    PLIB_CAN_ChannelForReceiveSet(CAN_ID_1, CAN_CHANNEL1, 32, CAN_RX_FULL_RECEIVE);
    PLIB_CAN_FilterToChannelLink(CAN_ID_1, CAN_FILTER0, CAN_FILTER_MASK0, CAN_CHANNEL1);
    PLIB_CAN_ChannelEventEnable(CAN_ID_1, CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY);
    /* Configure CAN_ID_1 Channel for CAN_TX_RTR_DISABLED operation. Allocate 32 message buffer, and assign low medium priority for transmissions. */
    PLIB_CAN_ChannelForTransmitSet(CAN_ID_1, CAN_CHANNEL2, 32, CAN_TX_RTR_DISABLED, CAN_LOW_MEDIUM_PRIORITY);
    PLIB_CAN_ChannelEventEnable(CAN_ID_1, CAN_CHANNEL2, CAN_RX_CHANNEL_NOT_EMPTY);
    /* Configure CAN_ID_1 Channel for CAN_RX_FULL_RECEIVE operation. Allocate 32 message buffer, and assign low medium priority for transmissions. */
    PLIB_CAN_ChannelForReceiveSet(CAN_ID_1, CAN_CHANNEL3, 32, CAN_RX_FULL_RECEIVE);
    PLIB_CAN_FilterToChannelLink(CAN_ID_1, CAN_FILTER1, CAN_FILTER_MASK1, CAN_CHANNEL3);
    PLIB_CAN_ChannelEventEnable(CAN_ID_1, CAN_CHANNEL3, CAN_RX_CHANNEL_NOT_EMPTY);
    PLIB_CAN_FilterConfigure(CAN_ID_1, CAN_FILTER0, 0x5F0, CAN_SID);
    PLIB_CAN_FilterEnable(CAN_ID_1, CAN_FILTER0);
    PLIB_CAN_FilterConfigure(CAN_ID_1, CAN_FILTER1, 0x187, CAN_SID);
    PLIB_CAN_FilterEnable(CAN_ID_1, CAN_FILTER1);

    PLIB_CAN_FilterMaskConfigure(CAN_ID_1, CAN_FILTER_MASK0, 0x7F0, CAN_SID, CAN_FILTER_MASK_IDE_TYPE);

    /* Switch the CAN module to Normal mode. Wait until the switch is complete */
    PLIB_CAN_OperationModeSelect(CAN_ID_1, CAN_NORMAL_MODE);
    while(PLIB_CAN_OperationModeGet(CAN_ID_1) != CAN_NORMAL_MODE);

    PLIB_CAN_ModuleEventEnable(CAN_ID_1 , 0);




    /* Setup CAN_ID_1 Interrupt */
    PLIB_INT_SourceEnable(INT_ID_0,INT_SOURCE_CAN_1);
    PLIB_INT_VectorPrioritySet(INT_ID_0,INT_VECTOR_CAN1, INT_PRIORITY_LEVEL5);
    PLIB_INT_VectorSubPrioritySet(INT_ID_0,INT_VECTOR_CAN1, INT_SUBPRIORITY_LEVEL0);
}

void DRV_CAN0_Deinitialize(void)
{

    /* Switch the CAN module to Disable mode. Wait until the switch is complete */
    PLIB_CAN_OperationModeSelect(CAN_ID_1, CAN_DISABLE_MODE);
    while(PLIB_CAN_OperationModeGet(CAN_ID_1) != CAN_DISABLE_MODE);

}

void DRV_CAN0_Open(void)
{
   /* Switch the CAN module ON */
    PLIB_CAN_Enable(CAN_ID_1);
}

void DRV_CAN0_Close(void)
{
   /* Switch the CAN module OFF */
    PLIB_CAN_Disable(CAN_ID_1);
}

bool DRV_CAN0_ChannelMessageTransmit(CAN_CHANNEL channelNum, int address, uint8_t DLC, uint8_t* message)
{

    int count = 0;
    if ( (PLIB_CAN_ChannelEventGet(CAN_ID_1, channelNum) & CAN_TX_CHANNEL_NOT_FULL) == CAN_TX_CHANNEL_NOT_FULL)
    {
        //drv_Message0 = (CAN_TX_MSG_BUFFER *) &can_message_buffer0[channelNum];
        drv_Message0 = PLIB_CAN_TransmitBufferGet(CAN_ID_1, channelNum);

        /* Check the address whether it falls under SID or EID,
         * SID max limit is 0x7FF, so anything beyond that is EID */
        if(address > 0x7FF)
        {
            drv_Message0->msgSID.sid     = (address>>18);
            drv_Message0->msgEID.eid     = (0x3FFFF)&(address);
            drv_Message0->msgEID.ide     = 1;
        }
        else
        {
            drv_Message0->msgSID.sid     = address;
            drv_Message0->msgEID.eid     = 0;
            drv_Message0->msgEID.ide     = 0;
        }
        if (DLC > 8)
        {
            DLC = 8;
        }

        drv_Message0->msgEID.data_length_code     = DLC;
        while(count < DLC)
        {
            drv_Message0->data[count++] = *message++;
        }

        //Update CAN module and then transmit data on the bus;
       PLIB_CAN_ChannelUpdate(CAN_ID_1, channelNum);
       PLIB_CAN_TransmitChannelFlush(CAN_ID_1, channelNum);
        return(true);
    }
    return (false);
}

bool DRV_CAN0_ChannelMessageReceive(CAN_CHANNEL channelNum, int address, uint8_t DLC, uint8_t* message)
{
    int EchoDLC = 0;
    CAN_RX_MSG_BUFFER *RxMessage;
    uint32_t tempAddress;
    bool readStatus;
    CAN_CHANNEL_EVENT ChannelEvent;

    /* Get the channel RX status */
    ChannelEvent = PLIB_CAN_ChannelEventGet( CAN_ID_1 , channelNum );

    /* Check if there is a message available to read. */
    if( (ChannelEvent & CAN_RX_CHANNEL_NOT_EMPTY) == CAN_RX_CHANNEL_NOT_EMPTY )
    {
        /* There is a message available in the Channel FIFO. */

        /* Get a pointer to RX message buffer */
        RxMessage = (CAN_RX_MSG_BUFFER *)PLIB_CAN_ReceivedMessageGet(CAN_ID_1, channelNum);

        /* Process the message fields */

        /* Check if it's a extended message type */
        if(RxMessage->msgEID.ide)
        {
            tempAddress = (RxMessage->msgSID.sid << 18);
            tempAddress |= ((0x3FFFF)&(RxMessage->msgEID.eid));
        }
        else
        {
            tempAddress = RxMessage->msgSID.sid;
        }


        if (RxMessage->msgEID.data_length_code > 0)
        {

            if(tempAddress == address)
            {
                while(EchoDLC < RxMessage->msgEID.data_length_code)
                {
                     *message++ = RxMessage->data[EchoDLC++];
                }
            }
        }

        /* Message processing is done, update the message buffer pointer. */
        PLIB_CAN_ChannelUpdate(CAN_ID_1, channelNum);

        /* Message is processed successfully, so return true */
        readStatus = true;
    }
    else
    {
        /* There is no message to read ,so return false */
        readStatus = false;
    }

    return readStatus;
}

/*******************************************************************************
 End of File
*/