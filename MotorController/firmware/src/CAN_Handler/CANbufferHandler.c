
#include "CANbufferHandler.h"
#include <stdint.h>

unsigned int CAN_buff_modulo_inc(const unsigned int value, const unsigned int modulus);

void buffer_put(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat) {


    if (_this->count < (BUFFER_SIZE_CAN - 3)) {

        _this->buf[_this->head] = towhere;
        _this->head = buffer_modulo_inc(_this->head, BUFFER_SIZE_CAN);
        ++_this->count;
        _this->buf[_this->head] = towhat;
        _this->head = buffer_modulo_inc(_this->head, BUFFER_SIZE_CAN);
        ++_this->count;
    }

}

void buffer_put_3(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat, unsigned int val) {
    if (_this->count < (BUFFER_SIZE_CAN - 3)) {

        _this->buf[_this->head] = towhere;
        _this->head = buffer_modulo_inc(_this->head, BUFFER_SIZE_CAN);
        ++_this->count;
        _this->buf[_this->head] = towhat;
        _this->head = buffer_modulo_inc(_this->head, BUFFER_SIZE_CAN);
        ++_this->count;
        _this->buf[_this->head] = val;
        _this->head = buffer_modulo_inc(_this->head, BUFFER_SIZE_CAN);
        ++_this->count;
    }

}


//pulls info out of the send buffer in a first in first out fashion

unsigned int buffer_get(ringBufCAN_t* _this) {
    unsigned int c;
    if (_this->count > 0) {
        c = _this->buf[_this->tail];
        _this->tail = buffer_modulo_inc(_this->tail, BUFFER_SIZE_CAN);
        --_this->count;
    } else {

        c = 0;
    }
    return (c);
}

unsigned int buffer_peek(ringBufCAN_t* _this) {
    unsigned int c;
    if (_this->count > 0) {
        c = _this->buf[_this->tail];
    } else {

        c = 0;
    }
    return (c);
}


//flushes the send buffer to get it ready for new data

void buffer_flush(ringBufCAN_t* _this, const int clearBuffer) {
    _this->count = 0;
    _this->head = 0;
    _this->tail = 0;
    if (clearBuffer) {

        wipeBuffer(_this->buf, 0, sizeof (_this->buf));
    }
}

void wipeBuffer(int*buf, int val, uint16_t numVals) {
    int i = 0;
    for (i = 0; i < numVals; i++) {

        buf[i] = val;
    }
}

// increments counters for the buffer functions

unsigned int buffer_modulo_inc(const unsigned int value, const unsigned int modulus) {
    unsigned int my_value = value + 1;
    if (my_value >= modulus) {

        my_value = 0;
    }
    return (my_value);
}

//getter for send circular buffer. 

unsigned int buffer_GetCount(ringBufCAN_t* _this) {

    return _this->count;
}


//**************************************************************
//                   CAN BUFFER FUNCTIONS
//**************************************************************



my_can_packet_t generateEmptyPacket(void);
/*=========================Generic UART Functions=============================*/
void CANwipeBuffer(my_can_buffer_t * canBuf)
{
    my_can_packet_t wipePacket = generateEmptyPacket();
    int i=0;
    for(i=0;i<CAN_BUFFER_PACKET_LENGTH;i++)
    {
        canBuf->buf[i]=wipePacket;
    }
    canBuf->head=0;
    canBuf->tail=0;
    canBuf->count=0;
}

void CANbufPut(my_can_buffer_t * canBuf, my_can_packet_t p)
{
     if (canBuf->count < CAN_BUFFER_PACKET_LENGTH)
    {
        canBuf->buf[canBuf->head] = p;
        canBuf->head = CAN_buff_modulo_inc(canBuf->head, CAN_BUFFER_PACKET_LENGTH);
        ++canBuf->count;
    } 
     else
    {
        canBuf->buf[canBuf->head] = p;
        canBuf->head = CAN_buff_modulo_inc(canBuf->head, CAN_BUFFER_PACKET_LENGTH);
        canBuf->tail = CAN_buff_modulo_inc(canBuf->tail, CAN_BUFFER_PACKET_LENGTH);

    }
}
unsigned int buff_get_tail_index(my_can_buffer_t * canBuf)
{
    return canBuf->tail;
}
unsigned int buff_get_head_index(my_can_buffer_t * canBuf)
{
    return canBuf->head;
}

unsigned int buff_get_last_index(my_can_buffer_t * canBuf)
{
    if(canBuf->tail == 0)
    {
        return CAN_BUFFER_PACKET_LENGTH-2;
    }
    else
    {
        return canBuf->tail-1;
    }
}

my_can_packet_t CANbufGet(my_can_buffer_t * canBuf)
{
    my_can_packet_t p;
    if (canBuf->count > 0)
    {
        p = canBuf->buf[canBuf->tail];
        canBuf->tail = CAN_buff_modulo_inc(canBuf->tail, CAN_BUFFER_PACKET_LENGTH);
        --canBuf->count;
    } 
    else
    {
      
        p =  generateEmptyPacket();
    }
    return (p);
}

my_can_packet_t generateEmptyPacket(void)
{
     my_can_packet_t wipePacket;
    wipePacket.canChannel=0;
    wipePacket.canAddress=0;
    wipePacket.DLC_Code=0;
    wipePacket.messageContents[0] = 0;
    wipePacket.messageContents[1] = 0;
    wipePacket.messageContents[2] = 0;
    wipePacket.messageContents[3] = 0;
    wipePacket.messageContents[4] = 0;
    wipePacket.messageContents[5] = 0;
    wipePacket.messageContents[6] = 0;
    wipePacket.messageContents[7] = 0;
    return wipePacket;
}
unsigned int CAN_buff_modulo_inc(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = value + 1;
    if (my_value >= modulus)
    {
        my_value = 0;
    }
    return (my_value);
}