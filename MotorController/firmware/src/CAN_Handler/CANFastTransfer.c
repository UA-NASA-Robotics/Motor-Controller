#include "CANFastTransfer.h"
#include "CANbufferHandler.h"
#include "GlobalCAN_IDs.h"
//0b0111 1111 11xx xxxx (where the x's are your address)
#define LAST_BOARD_RECEIEVED 0


void beginCANFast(volatile short * ptr, volatile bool *flagPtr, unsigned int maxSize, unsigned char givenAddress, FT_type_t _t);
void buffer_put(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat);

void setNewDataFlag(FT_type_t _t, int index);

ringBufCAN_t transmit_buffer_CAN, send_buffer_CAN_FT, rx_buffer_CAN;
ringBufCAN_t rx_buffer_CAN_Global;

bool dataReceived[2] = {false,false};
volatile short * receiveArrayAddressCAN[2];
volatile bool * receiveArrayAddressCAN_Flag[2];
unsigned char moduleAddressCAN[2];
unsigned int MaxIndex[2];
short receiveArrayCAN[10];
bool receiveArrayCAN_Flag[10];

int newDataFlag = 0;
int newDataFlag_Global[2];

short * getReceiveArrayCAN(void) {
    return receiveArrayCAN;
}

void initCANFT(void) {
    beginCANFast(receiveArrayCAN, receiveArrayCAN_Flag, sizeof (receiveArrayCAN), MY_ADDRESS,FT_LOCAL);
    beginCANFast(receiveArrayCAN_Global, GBL_CAN_FT_recievedFlag, sizeof (receiveArrayCAN_Global), GLOBAL_ADDRESS, FT_GLOBAL);
}

void beginCANFast(volatile short * ptr, volatile bool *flagPtr, unsigned int maxSize, unsigned char givenAddress, FT_type_t _t) {

    receiveArrayAddressCAN[_t] = ptr;
    receiveArrayAddressCAN_Flag[_t] = flagPtr;
    moduleAddressCAN[_t] = givenAddress;
    MaxIndex[_t] = maxSize;

    if (_t == FT_GLOBAL) {
        buffer_flush(&rx_buffer_CAN_Global, 0);
    } else {
        buffer_flush(&rx_buffer_CAN, 0);
        buffer_flush(&send_buffer_CAN_FT, 0);
        buffer_flush(&transmit_buffer_CAN, 0);
    }
}

void setNewDataFlag(FT_type_t _t, int index) {
    (receiveArrayAddressCAN_Flag[_t])[index] = 1;
}

void clearNewDataFlag(FT_type_t _t, int index) {
    (receiveArrayAddressCAN_Flag[_t])[index] = 0;
}

bool getNewDataFlagStatus(FT_type_t _t, int index) {
    if ((receiveArrayAddressCAN_Flag[_t])[index]) {
        clearNewDataFlag(_t, index);
        return true;
    } else {
        return false;
    }
}

int getCANFastData(FT_type_t _t, uint8_t index) {
    ReceiveDataCAN(_t);
    if (index < MaxIndex[_t]) {

        return (receiveArrayAddressCAN[_t])[index];
    }
    return 0xFFFF;
}

void clearCANFastDataValue(int index) {

    receiveArrayCAN[index] = 0;
}

void clearCANFastDataValueRange(int startIndex, int end) {
    if (startIndex > end) {
        int index = startIndex;
        startIndex = end;
        end = index;
    }
    int i;
    for (i = startIndex; i <= end; i++) {

        receiveArrayCAN[i] = 0;
        clearNewDataFlag(FT_LOCAL, i);
    }
}

void ReceiveCANFast(my_can_packet_t *p, FT_type_t _t) // interrupt callback
{
    ringBufCAN_t *rx_Buff;
    if (_t == FT_LOCAL) 
        rx_Buff = &rx_buffer_CAN;
    else 
        rx_Buff = &rx_buffer_CAN_Global;

    if ((p->canAddress >> 6) == moduleAddressCAN[_t]) {
        if (p->DLC_Code == 8 || p->DLC_Code == 9) //Check number of bytes, if 8 read in two ints
        {
            if ((p->messageContents[0] << 8) +(p->messageContents[1]) < MaxIndex[_t]) {
                buffer_put(rx_Buff, (p->messageContents[0] << 8) +(p->messageContents[1]), (p->messageContents[2] << 8) +(p->messageContents[3]));
                dataReceived[_t] = 1;
            }
            if ((p->messageContents[4] << 8) +(p->messageContents[5]) < MaxIndex[_t]) {
                buffer_put(rx_Buff, (p->messageContents[4] << 8) +(p->messageContents[5]), (p->messageContents[6] << 8) +(p->messageContents[7]));
                dataReceived[_t] = 1;
            }
            buffer_put(rx_Buff, LAST_BOARD_RECEIEVED, (p->canAddress & 0b11111));
        } else //else read in one int
        {
            if ((p->messageContents[0] << 8) +(p->messageContents[1]) < MaxIndex[_t]) {

                buffer_put(rx_Buff, (p->messageContents[0] << 8) +(p->messageContents[1]), (p->messageContents[2] << 8) +(p->messageContents[3]));
                dataReceived[_t] = 1;
            }
            buffer_put(rx_Buff, LAST_BOARD_RECEIEVED, (p->canAddress & 0b11111));
        }
    }
}

int ReceiveDataCAN(FT_type_t _t) {
    ringBufCAN_t *rx_Buff;
    if (_t == FT_LOCAL) rx_Buff = &rx_buffer_CAN;
    else rx_Buff = &rx_buffer_CAN_Global;

    if (dataReceived[_t]) {
        dataReceived[_t] = 0;
        int i = buffer_GetCount(rx_Buff);
        if (i) //this better be true ... if not in instant receive 
        {
            for (; i > 0; i = i - 2) {
                int address = buffer_get(rx_Buff);
                (receiveArrayAddressCAN[_t])[address] = buffer_get(rx_Buff);
                setNewDataFlag(_t, address);
            }
            return 1;
        } else {
            //error (how was ReceiveData true if no data available)
            return 0;
        }

    } else
        return 0;
}

bool TransmitCANFast(my_can_packet_t *p) // interrupt callback
{
    if (buffer_GetCount(&transmit_buffer_CAN) > 0) {

        //if more than 2 data/index pairs left might be able to send large packet. 
        if (buffer_GetCount(&transmit_buffer_CAN) > 6) {
            unsigned int address = buffer_get(&transmit_buffer_CAN);
            p->canAddress = (address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
            //we are good to send the first index/value pair for sure. 
            int i = 0;
            for (i = 0; i < 2; i++) {
                unsigned int temp = buffer_get(&transmit_buffer_CAN);
                p->messageContents[2 * i] = (temp >> 8);
                p->messageContents[2 * i + 1] = temp;
            }
            //now we need to check that the next address matches the current address
            if (buffer_peek(&transmit_buffer_CAN) == address) {
                //address match and we are clear to send them in a single packet
                buffer_get(&transmit_buffer_CAN); //call this to clear out the address
                int i = 0;
                for (i = 2; i < 4; i++) {
                    unsigned int temp = buffer_get(&transmit_buffer_CAN);
                    p->messageContents[2 * i] = (temp >> 8);
                    p->messageContents[2 * i + 1] = temp;
                }
                //need to check if this was the last packet 
                if (buffer_peek(&transmit_buffer_CAN) == address) {
                    //next address is same, so this isn't the last packet, send a length 8.
                    p->DLC_Code = 8;
                } else {
                    //next address is different, and this is the last packet, so send a 9.
                    p->DLC_Code = 9;
                }
            } else {
                //addresses don't match and we should just send what is currently in the packet. 
                p->DLC_Code = 4;
            }
        }//if exactly 2 data/index pairs left send with length 9. Receiver
            //will read the "wrong" length correctly, but realize this is the last packet.
            //note: still need to check incase two different destinations. 
        else if (buffer_GetCount(&transmit_buffer_CAN) == 6) {
            unsigned int address = buffer_get(&transmit_buffer_CAN);
            p->canAddress = (address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
            //we are good to send the first index/value pair for sure.
            int i = 0;
            for (i = 0; i < 2; i++) {
                unsigned int temp = buffer_get(&transmit_buffer_CAN);
                p->messageContents[2 * i] = (temp >> 8); //this is so dumb
                p->messageContents[2 * i + 1] = temp;
            }
            //now we need to check that the next address matches the current address
            if (buffer_peek(&transmit_buffer_CAN) == address) {
                //address match and we are clear to send them in a single packet
                buffer_get(&transmit_buffer_CAN); //call this to clear out the address
                int i = 0;
                for (i = 2; i < 4; i++) {
                    unsigned int temp = buffer_get(&transmit_buffer_CAN);
                    p->messageContents[2 * i] = (temp >> 8);
                    p->messageContents[2 * i + 1] = temp;
                }
                p->DLC_Code = 9;
            } else {
                //addresses don't match and we should just send what is currently in the packet.
                p->DLC_Code = 4;
            }

        }//if only 1 data/index pair receiver will know it is the last packet.
        else if (buffer_GetCount(&transmit_buffer_CAN) == 3) {
            unsigned int address = buffer_get(&transmit_buffer_CAN);
            p->canAddress = (address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
            p->DLC_Code = 4;
            int i = 0;
            for (i = 0; i < 2; i++) {
                unsigned int temp = buffer_get(&transmit_buffer_CAN);
                p->messageContents[2 * i] = (temp >> 8);
                p->messageContents[2 * i + 1] = temp;
            }
        } else {
            //error, missing dest/index/value set, previous packets could be very corrupt
            //TransmitSetMissMatch++;
            buffer_flush(&transmit_buffer_CAN, 1);
            return false;
        }
        return true;
    } else {
        C1FIFOINT2bits.TXEMPTYIE = 0;

        return false;
    }
}

void ToSendCAN(unsigned int where, unsigned int what) {

    buffer_put(&send_buffer_CAN_FT, where, what);
}

void sendDataCAN(unsigned int whereToSend) {
    //NEW STATIC METHOD
    int temp = buffer_GetCount(&send_buffer_CAN_FT); //get size of things to send

    int i = 0;
    for (i = 0; i < (temp >> 1); i++) { //need to divid by two since reading index/value pairs, hence >>1
        int index = buffer_get(&send_buffer_CAN_FT);
        int value = buffer_get(&send_buffer_CAN_FT);
        buffer_put_3(&transmit_buffer_CAN, whereToSend, index, value);
    }
    if (C1FIFOINT2bits.TXEMPTYIE == 0) {

        C1FIFOINT2bits.TXEMPTYIE = 1;
    }
}



int GlobalAddressInturpret(int index) {
    return MY_ADDRESS * GLOBAL_SYSTEM_DATA_SIZE + index;
}