/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include "CAN.h"
//#include <proc/p32mz2048efh.h>
#include "../../../../../framework/driver/can/drv_can.h"
#include "Definitions.h"
#include "Timers.h"
#include "motorHandler.h"
#include "CAN_Handler/CANFastTransfer.h"

#define CHANNEL_0_CAN 0
#define CHANNEL_1_CAN 1
#define CHANNEL_2_CAN 2
#define CHANNEL_3_CAN 3
#define CHANNEL_4_CAN 4


typedef struct {
    int rx_requested;
    Data_t *callback;
    uint16_t CANID;
    uint16_t contentsID;
} rx_message_tracking_t;

rx_message_tracking_t rx_message_requested[CAN_BUFFER_PACKET_LENGTH];
my_can_buffer_t CAN_tx_buffer;
my_can_buffer_t CAN_rx_buffer;
my_can_buffer_t tx_FT_buffer;
my_can_buffer_t rx_FT_buffer;
int lastMessageIndexSent = 0;

bool transmitStallCAN = true;
bool transmitStallCAN_FT = true;
volatile bool CANRxAvailable = false;
volatile bool awaitingResponse = false;
volatile bool CANRxAvailable_FT = false;

void copyMessageArrays(uint8_t * intoThis, uint8_t * fromThis);

void initCANISRs(void) {
    CANwipeBuffer(&CAN_tx_buffer);
    CANwipeBuffer(&CAN_rx_buffer);
    C1FIFOINT1bits.RXNEMPTYIE = 1; //Interrupt when the RX has something in it
    //C1FIFOINT0bits.TXEMPTYIE    =   1;  //Interrupt when the TX has room available


    CANwipeBuffer(&tx_FT_buffer);
    CANwipeBuffer(&rx_FT_buffer);
    C1FIFOINT3bits.RXNEMPTYIE = 1; //Interrupt when the RX has something in it
    //C1FIFOINT0bits.TXEMPTYIE    =   1;  //Interrupt when the TX has room available

    //Init the rx request tracking holder
    int i = 0;
    for (i = 0; i < CAN_BUFFER_PACKET_LENGTH; i++) {
        rx_message_requested[i].CANID = 0;
        rx_message_requested[i].rx_requested = 0;
    }

    C1INTbits.RBIE = 1; //Enable all RX interrupts to trigger an interrupt
    C1INTbits.TBIE = 1; //Enable all transmit interrupts
}

void CAN_ISR_CALLBACK(void) {
    /*** CAN FAST_TRANSFER RECEIVE (local & Global)***/
    if (C1FIFOINT3bits.RXNEMPTYIF || C1FIFOINT4bits.RXNEMPTYIF) //RBIF
    {
        int CHANNEL;
        if (C1FIFOINT3bits.RXNEMPTYIF) {
            //System Receive
            CHANNEL = CHANNEL_3_CAN;
        } else {
            //Global Receive
            CHANNEL = CHANNEL_4_CAN;
        }

        CAN_RX_MSG_BUFFER * RxMessage;
        /* Get the channel RX status */

        CAN_CHANNEL_EVENT ChannelEvent;
        ChannelEvent = PLIB_CAN_ChannelEventGet(CAN_ID_1, CHANNEL);

        /* Check if there is a message available to read. */
        if ((ChannelEvent & CAN_RX_CHANNEL_NOT_EMPTY) == CAN_RX_CHANNEL_NOT_EMPTY) {
            RxMessage = (CAN_RX_MSG_BUFFER *) PLIB_CAN_ReceivedMessageGet(CAN_ID_1, CHANNEL);
        }
        my_can_packet_t rxPacket;

        rxPacket.canChannel = CHANNEL;
        rxPacket.canAddress = (uint16_t) RxMessage->msgSID.sid;
        copyMessageArrays(rxPacket.messageContents, RxMessage->data);

        //CANbufPut(&rx_FT_buffer,rxPacket);
        ReceiveCANFast(&rxPacket,(CHANNEL == CHANNEL_3_CAN? FT_LOCAL : FT_GLOBAL));
        /* Message processing is done, update the message buffer pointer. */
        PLIB_CAN_ChannelUpdate(CAN_ID_1, CHANNEL);
        CANRxAvailable_FT = true;

    }
    /*** CAN FAST_TRANSFER TRANSMIT ***/
    if (C1FIFOINT2bits.TXEMPTYIF) //TBIF
    {
        my_can_packet_t sendingNext; // = CANbufGet(&tx_FT_buffer);
        if (TransmitCANFast(&sendingNext))
            DRV_CAN0_ChannelMessageTransmit(CHANNEL_2_CAN, sendingNext.canAddress, sendingNext.DLC_Code, sendingNext.messageContents);

    }

    /*** MOTORS RECEIVE ***/
    if (C1FIFOINT1bits.RXNEMPTYIF) //RBIF
    {

        CAN_RX_MSG_BUFFER * RxMessage;
        /* Get the channel RX status */

        CAN_CHANNEL_EVENT ChannelEvent;
        ChannelEvent = PLIB_CAN_ChannelEventGet(CAN_ID_1, CHANNEL_1_CAN);

        /* Check if there is a message available to read. */
        if ((ChannelEvent & CAN_RX_CHANNEL_NOT_EMPTY) == CAN_RX_CHANNEL_NOT_EMPTY) {
            RxMessage = (CAN_RX_MSG_BUFFER *) PLIB_CAN_ReceivedMessageGet(CAN_ID_1, CHANNEL_1_CAN);
        } else {

        }
        my_can_packet_t rxPacket;

        rxPacket.canChannel = 1;
        rxPacket.canAddress = RxMessage->msgSID.sid;
        copyMessageArrays(rxPacket.messageContents, RxMessage->data);

        //Should contain the length identifier
        rxPacket.DLC_Code = rxPacket.messageContents[0]; //RX contents ( 0x41=4 Byte, 0x43=4Byte, 0x4B=2 Byte, 0x4F=1 Byte) TX contents (0x60))

        //        printf("rxAddr %x\r",rxPacket.canAddress);
        //        if((char)(rxPacket.canAddress & 0x007F)==0x7B)
        //        {
        //            printf("PlowMatch\r");
        //            printf("%x, " rxPacket.canAddress);
        //            int jk=0;
        //            for(jk=0;jk<8;jk++)
        //            {            
        //                printf("%x,",rxPacket.messageContents[jk]);
        //            }
        //            printf("/r");
        //        }

        //Look for requested data
        uint16_t dataAddressPacket = (uint16_t) ((((uint16_t) rxPacket.messageContents[2]) << 8) + rxPacket.messageContents[1]);
        //        uint16_t dataAddressPacket = (uint16_t)((((uint16_t)rxPacket.messageContents[2])<<8) | rxPacket.messageContents[1]);  

        if ((rx_message_requested[lastMessageIndexSent].CANID == rxPacket.canAddress) && (rx_message_requested[lastMessageIndexSent].contentsID == dataAddressPacket)) {
            //Store a version of the address that we use (0x7_)
            uint8_t comparableAddress = (uint8_t) (rxPacket.canAddress & 0x007F);


            //Combine the data into a 32bit format
            //            long dataCombination = (long)((((long)rxPacket.messageContents[7])<<24) +(((long)rxPacket.messageContents[6])<<16) + (((long)rxPacket.messageContents[5])<<8) + rxPacket.messageContents[4]);   

            long dataCombination = 0;
            switch (rxPacket.DLC_Code) {
                case 0x41:
                case 0x43:
                    dataCombination = (long) ((((long) rxPacket.messageContents[7]) << 24) +(((long) rxPacket.messageContents[6]) << 16) + (((long) rxPacket.messageContents[5]) << 8) + rxPacket.messageContents[4]);
                    break;
                case 0x4B:
                    dataCombination = (long) ((((int16_t) rxPacket.messageContents[5]) << 8) + rxPacket.messageContents[4]);
                    break;
                case 0x4F:
                    dataCombination = (long) rxPacket.messageContents[4];
                    break;
            }
            //            printf("RXData: %d\r",dataCombination);
            bool returnable = false;
            if(rx_message_requested[lastMessageIndexSent].rx_requested != NO_DATA_REQUESTED && rx_message_requested[lastMessageIndexSent].callback != NULL)
            {
                ((Data_t*)(rx_message_requested[lastMessageIndexSent].callback))->data = dataCombination;
                ((Data_t*)(rx_message_requested[lastMessageIndexSent].callback))->newDataFlag = true;
            }
            //If you wanted data back, store it according to the type requested
//            switch (rx_message_requested[lastMessageIndexSent].rx_requested) {
//                case NO_DATA_REQUESTED:
//
//                    break;
//                case ERROR_REQUESTED:
//                    switch (comparableAddress) {
//                        case RIGHTMOTORID:
//                            storeMotorError(&RightMotor, dataCombination);
//                            break;
//                        case LEFTMOTORID:
//                            storeMotorError(&LeftMotor, dataCombination);
//                            break;
//                        case DRUMMOTORID:
//                            storeMotorError(&DrumMotor, dataCombination);
//                            break;
//                        case ARMMOTORID:
//                            storeMotorError(&ArmMotor, dataCombination);
//                            break;
//                        case PLOWMOTORID:
//                            storeMotorError(&PlowMotor, dataCombination);
//                            break;
//                    }
//                    break;
//                case ENCODER_POSITION_REQUESTED:
//                case HALL_POSITION_REQUESTED:
//                case SSI_ENCODER_POSITION_REQUESTED:
//                    switch (comparableAddress) {
//                        case RIGHTMOTORID:
//                            storeMotorPosition(&RightMotor, dataCombination);
//                            break;
//                        case LEFTMOTORID:
//                            storeMotorPosition(&LeftMotor, dataCombination);
//                            break;
//                        case DRUMMOTORID:
//                            storeMotorPosition(&DrumMotor, dataCombination);
//                            break;
//                        case ARMMOTORID:
//                            storeMotorPosition(&ArmMotor, dataCombination);
//                            break;
//                        case PLOWMOTORID:
//                            storeMotorPosition(&PlowMotor, dataCombination);
//                            break;
//                    }
//                    //CANbufPut(&CAN_rx_buffer,rxPacket);
//                    break;
//
//                case ANALOG_0_REQUESTED:
//                    switch (comparableAddress) {
//                        case RIGHTMOTORID:
//                            storeMotorAnalog0(&RightMotor, (int16_t) dataCombination);
//                            break;
//                        case LEFTMOTORID:
//                            storeMotorAnalog0(&LeftMotor, (int16_t) dataCombination);
//                            break;
//                        case DRUMMOTORID:
//                            storeMotorAnalog0(&DrumMotor, (int16_t) dataCombination);
//                            break;
//                        case ARMMOTORID:
//                            storeMotorAnalog0(&ArmMotor, (int16_t) dataCombination);
//                            break;
//                        case PLOWMOTORID:
//                            storeMotorAnalog0(&PlowMotor, (int16_t) dataCombination);
//                            break;
//                    }
//                    break;
//                case ANALOG_1_REQUESTED:
//                    switch (comparableAddress) {
//                        case RIGHTMOTORID:
//                            storeMotorAnalog1(&RightMotor, (int16_t) dataCombination);
//                            break;
//                        case LEFTMOTORID:
//                            storeMotorAnalog1(&LeftMotor, (int16_t) dataCombination);
//                            break;
//                        case DRUMMOTORID:
//                            storeMotorAnalog1(&DrumMotor, (int16_t) dataCombination);
//                            break;
//                        case ARMMOTORID:
//                            storeMotorAnalog1(&ArmMotor, (int16_t) dataCombination);
//                            break;
//                        case PLOWMOTORID:
//                            storeMotorAnalog1(&PlowMotor, (int16_t) dataCombination);
//                            break;
//                    }
//                    break;
//                case DIGITAL_INPUT_REQUESTED:
//                    switch (comparableAddress) {
//                        case RIGHTMOTORID:
//                            storeMotorDigitalInputs(&RightMotor, (uint8_t) dataCombination);
//                            break;
//                        case LEFTMOTORID:
//                            storeMotorDigitalInputs(&LeftMotor, (uint8_t) dataCombination);
//                            break;
//                        case DRUMMOTORID:
//                            storeMotorDigitalInputs(&DrumMotor, (uint8_t) dataCombination);
//                            break;
//                        case ARMMOTORID:
//                            storeMotorDigitalInputs(&ArmMotor, (uint8_t) dataCombination);
//                            //                            printf("dataComb: %u\r",(uint8_t)dataCombination);      
//                            //                            printf("packet4: %u\r",(uint8_t)rxPacket.messageContents[4]);                          
//                            break;
//                        case PLOWMOTORID:
//                            storeMotorDigitalInputs(&PlowMotor, (uint8_t) dataCombination);
//                            break;
//                    }
//                    break;
//                case POSITION_REACHED_REQUESTED:
//                    if ((dataCombination & 0x10) > 0) {
//                        returnable = true;
//                    }
//                    switch (comparableAddress) {
//                        case RIGHTMOTORID:
//                            storeMotorPositionReached(&RightMotor, returnable);
//                            break;
//                        case LEFTMOTORID:
//                            storeMotorPositionReached(&LeftMotor, returnable);
//                            break;
//                        case DRUMMOTORID:
//                            storeMotorPositionReached(&DrumMotor, returnable);
//                            break;
//                        case ARMMOTORID:
//                            storeMotorPositionReached(&ArmMotor, returnable);
//                            break;
//                        case PLOWMOTORID:
//                            storeMotorPositionReached(&PlowMotor, returnable);
//                            break;
//                    }
//                    break;
//                case CURRENT_USAGE:
//                    switch (comparableAddress) {
//                        case RIGHTMOTORID:
//                            storeMotorCurrent(&RightMotor, (int16_t) dataCombination);
//                            break;
//                        case LEFTMOTORID:
//                            storeMotorCurrent(&LeftMotor, (int16_t) dataCombination);
//                            break;
//                        case DRUMMOTORID:
//                            storeMotorCurrent(&DrumMotor, (int16_t) dataCombination);
//                            break;
//                        case ARMMOTORID:
//                            storeMotorCurrent(&ArmMotor, (int16_t) dataCombination);
//                            break;
//                        case PLOWMOTORID:
//                            storeMotorCurrent(&PlowMotor, (int16_t) dataCombination);
//                            break;
//                    }
//                    break;
//            }
        }

        /* Message processing is done, update the message buffer pointer. */
        PLIB_CAN_ChannelUpdate(CAN_ID_1, CHANNEL_1_CAN);
        CANRxAvailable = true;

        //printf("1: ");
        //If we are waiting to send TX messages 
        if ((CAN_tx_buffer.count > 0) && (C1FIFOINT0bits.TXEMPTYIE == 0) && awaitingResponse) {
            //printf("%d vs. %d\r",rx_message_requested[lastMessageIndexSent].CANID,rxPacket.canAddress);
            //If the message was received as requested, move to sending the next
            if (rx_message_requested[lastMessageIndexSent].CANID == rxPacket.canAddress) {
                //Wipe the buffer when finishing this section
                rx_message_requested[lastMessageIndexSent].rx_requested = NO_DATA_REQUESTED;
                rx_message_requested[lastMessageIndexSent].callback = NULL;
                rx_message_requested[lastMessageIndexSent].CANID = 0;

                C1FIFOINT0bits.TXEMPTYIE = 1;
                awaitingResponse = false;
            }
        }
        else if (C1FIFOINT0bits.TXEMPTYIE == 0 && awaitingResponse) {
            //printf("2: ");
            if (rx_message_requested[lastMessageIndexSent].CANID == rxPacket.canAddress) {
                //printf("%d vs. %d\r",rx_message_requested[lastMessageIndexSent].CANID,rxPacket.canAddress);
                awaitingResponse = false;
            }
        } else {
            //printf("3: ");
            if (rx_message_requested[lastMessageIndexSent].CANID == rxPacket.canAddress) {
                //printf("%d vs. %d\r",rx_message_requested[lastMessageIndexSent].CANID,rxPacket.canAddress);
                awaitingResponse = false;
            }
        }

    }
    if (C1FIFOINT0bits.TXEMPTYIF) //TBIF
    {
        if (CAN_tx_buffer.count > 0 && !awaitingResponse) //More left to send
        {
            lastMessageIndexSent = buff_get_tail_index(&CAN_tx_buffer);
            my_can_packet_t sendingNext = CANbufGet(&CAN_tx_buffer);
            DRV_CAN0_ChannelMessageTransmit(sendingNext.canChannel, sendingNext.canAddress, sendingNext.DLC_Code, sendingNext.messageContents);
            C1FIFOINT0bits.TXEMPTYIE = 0;
            awaitingResponse = true;
            //printf("send: %d\r",sendingNext.canAddress);
        } else if (!awaitingResponse) {
            transmitStallCAN = true;
            C1FIFOINT0bits.TXEMPTYIE = 0;
        } else {
            C1FIFOINT0bits.TXEMPTYIE = 0;
        }
    }
}

void messageTransmit(my_can_packet_t packetToSend) {
    rx_message_requested[buff_get_head_index(&CAN_tx_buffer)].rx_requested = NO_DATA_REQUESTED;
    rx_message_requested[buff_get_head_index(&CAN_tx_buffer)].callback = NULL;
    rx_message_requested[buff_get_head_index(&CAN_tx_buffer)].CANID = (packetToSend.canAddress & 0x000F) + 0x5F0;
    CANbufPut(&CAN_tx_buffer, packetToSend);
    if (transmitStallCAN) {
        transmitStallCAN = false;
        //DRV_CAN0_ChannelMessageTransmit(packetToSend.canChannel,packetToSend.canAddress,packetToSend.DLC_Code,packetToSend.messageContents); 
    } else {
        //awaitingResponse = true; 
    }
    C1FIFOINT0bits.TXEMPTYIE = 1;
}

//responseType-
//A zero will not trigger a response
//This field will allow for messages to be auto sorted by response type
//Currently this field is under utilized

void messageTransmitWithResponse(my_can_packet_t packetToSend, int responseType, Data_t *_callbackPtr) {
    rx_message_requested[buff_get_head_index(&CAN_tx_buffer)].rx_requested = responseType;
    rx_message_requested[buff_get_head_index(&CAN_tx_buffer)].callback = _callbackPtr;
    
    rx_message_requested[buff_get_head_index(&CAN_tx_buffer)].CANID = (packetToSend.canAddress & 0x000F) + 0x5F0;
    rx_message_requested[buff_get_head_index(&CAN_tx_buffer)].contentsID = (((uint16_t) packetToSend.messageContents[2]) << 8) + packetToSend.messageContents[1];
    CANbufPut(&CAN_tx_buffer, packetToSend);

    if (transmitStallCAN) {
        transmitStallCAN = false;
        //DRV_CAN0_ChannelMessageTransmit(packetToSend.canChannel,packetToSend.canAddress,packetToSend.DLC_Code,packetToSend.messageContents); 
    } else {
        //awaitingResponse = true; 
    }
    C1FIFOINT0bits.TXEMPTYIE = 1;
}

void messageTransmitFT(my_can_packet_t packetToSend) {
    CANbufPut(&tx_FT_buffer, packetToSend);

    if (transmitStallCAN_FT) {
        transmitStallCAN_FT = false;
        //DRV_CAN0_ChannelMessageTransmit(packetToSend.canChannel,packetToSend.canAddress,packetToSend.DLC_Code,packetToSend.messageContents); 
    } else {
        //awaitingResponse = true; 
    }
    C1FIFOINT2bits.TXEMPTYIE = 1;
}

bool CANmessageAvailable(void) {
    return CANRxAvailable;
}

bool messageReceive(my_can_packet_t * packetToReceive) {
    *packetToReceive = CANbufGet(&CAN_rx_buffer);
    if (CAN_rx_buffer.count == 0) {
        CANRxAvailable = false;
    }
    if (packetToReceive->DLC_Code == 0)
        return false; //Should indicate that it wasn't populated 
    else
        return true;
}

//typedef struct{
//    uint8_t canChannel;
//    uint16_t canAddress;
//    uint8_t DLC_Code;
//    uint8_t messageContents[8];
//}my_can_packet_t;

void populateMotorPacketWrite(uint8_t * messageData, SDO_PACKET_t packetToSend) {
    messageData[0] = 0x23;
    messageData[1] = packetToSend.objAddr;
    messageData[2] = packetToSend.objAddr >> 8;
    messageData[3] = packetToSend.subAddr;
    messageData[4] = packetToSend.data;
    messageData[5] = packetToSend.data >> 8;
    messageData[6] = packetToSend.data >> 16;
    messageData[7] = packetToSend.data >> 24;
}

void populateMotorPacketRead(uint8_t * messageData, SDO_PACKET_t packetToSend) {
    messageData[0] = 0x40;
    messageData[1] = packetToSend.objAddr;
    messageData[2] = packetToSend.objAddr >> 8;
    messageData[3] = packetToSend.subAddr;
    messageData[4] = packetToSend.data;
    messageData[5] = packetToSend.data >> 8;
    messageData[6] = packetToSend.data >> 16;
    messageData[7] = packetToSend.data >> 24;
}

//we really dont want to wait for this data to be accepted, find a clever way of ensuring data success vs a while loop

void sendMotorPacket(uint8_t motorAddress, uint16_t objectAddr, uint8_t subAddr, long data) {
    my_can_packet_t sending;
    sending.canChannel = 0;
    sending.canAddress = 0x600 + motorAddress;
    sending.DLC_Code = DLC_VALUE;

    SDO_PACKET_t sendCommand = {objectAddr, subAddr, data};
    populateMotorPacketWrite(sending.messageContents, sendCommand);

    messageTransmit(sending);
}

//This will trigger the message system to log the response into the rx buffer
//Should include some response sorting potentially
//Could have structures built with each motor to hold counts/position/etc.

//void sendMotorPacketWithResponse(uint8_t motorAddress, uint16_t objectAddr, uint8_t subAddr, long data, int responseType) {
//    my_can_packet_t sending;
//    sending.canChannel = 0;
//    sending.canAddress = 0x600 + motorAddress;
//    sending.DLC_Code = DLC_VALUE;
//
//    SDO_PACKET_t sendCommand = {objectAddr, subAddr, data};
//    populateMotorPacketWrite(sending.messageContents, sendCommand);
//
//    messageTransmitWithResponse(sending, responseType);
//}

void requestMotorPacketWithResponse(uint8_t motorAddress, uint16_t objectAddr, uint8_t subAddr, long data, int responseType, Data_t *_callback) {
    my_can_packet_t sending;
    sending.canChannel = 0;
    sending.canAddress = 0x600 + motorAddress;
    sending.DLC_Code = DLC_VALUE;

    SDO_PACKET_t sendCommand = {objectAddr, subAddr, data};
    populateMotorPacketRead(sending.messageContents, sendCommand);

    messageTransmitWithResponse(sending, responseType, _callback);
}

//we really dont MIND waiting for this data to be accepted, during init we can wait all day

void sendMotorPacketInit(uint8_t motorAddress, uint16_t objectAddr, uint8_t subAddr, long data) {
    my_can_packet_t sending;
    sending.canChannel = 0;
    sending.canAddress = 0x600 + motorAddress;
    sending.DLC_Code = DLC_VALUE;

    SDO_PACKET_t sendCommand = {objectAddr, subAddr, data};
    populateMotorPacketWrite(sending.messageContents, sendCommand);

    messageTransmit(sending);

    unsigned long i, j;
    for (i = 0; i < 500000; i++) {
        j++;
    }

    //Pull the message out of the buffer for space saving and tidiness (hand it the variable we made earlier(dummy var))
    messageReceive(&sending);
}

void copyMessageArrays(uint8_t * intoThis, uint8_t * fromThis) {
    int i = 0;
    for (i = 0; i < 8; i++) {
        intoThis[i] = fromThis[i];
    }
}

/* *****************************************************************************
 End of File
 */