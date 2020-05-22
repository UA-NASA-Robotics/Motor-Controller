/* 
 * File:   CANbufferHandler.h
 * Author: John
 *
 * Created on November 15, 2019, 5:50 PM
 */

#ifndef CANBUFFERHANDLER_H
#define	CANBUFFERHANDLER_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdint.h>
#define CAN_BUFFER_PACKET_LENGTH 200  //Store 200 packets in the buffer

#define BUFFER_SIZE_CAN 200

typedef struct {
        int buf[BUFFER_SIZE_CAN];
        int head;
        int tail;
        int count;
    } ringBufCAN_t;

    typedef struct {
        uint8_t canChannel;
        uint16_t canAddress;
        uint8_t DLC_Code;
        uint8_t messageContents[8];
    } my_can_packet_t;

    typedef struct {
        my_can_packet_t buf[CAN_BUFFER_PACKET_LENGTH];
        uint16_t head;
        uint16_t tail;
        uint16_t count;
    } my_can_buffer_t;


    void buffer_put_3(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat, unsigned int val);
    unsigned int buffer_get(ringBufCAN_t* _this);
    unsigned int buffer_peek(ringBufCAN_t* _this);
    void buffer_flush(ringBufCAN_t* _this, const int clearBuffer);
    void wipeBuffer(int*buf, int val, uint16_t numVals);
    unsigned int buffer_modulo_inc(const unsigned int value, const unsigned int modulus);
    unsigned int buffer_GetCount(ringBufCAN_t* _this);

    void CANwipeBuffer(my_can_buffer_t * canBuf);
    void CANbufPut(my_can_buffer_t * canBuf, my_can_packet_t p);
    my_can_packet_t CANbufGet(my_can_buffer_t * canBuf);
    unsigned int buff_get_tail_index(my_can_buffer_t * canBuf);
    unsigned int buff_get_head_index(my_can_buffer_t * canBuf);
    unsigned int buff_get_last_index(my_can_buffer_t * canBuf);


#ifdef	__cplusplus
}
#endif

#endif	/* CANBUFFERHANDLER_H */

