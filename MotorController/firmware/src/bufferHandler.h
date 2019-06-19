/* 
 * File:   bufferHandler.h
 * Author: Seth Carpenter
 *
 * Created on March 22, 2018, 3:30 PM
 */

#ifndef BUFFERHANDLER_H
#define	BUFFERHANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


//#define StaticBuffers
#define BufferSize 100

#define CAN_BUFFER_PACKET_LENGTH 200  //Store 200 packets in the buffer

typedef struct{
    uint8_t canChannel;
    uint16_t canAddress;
    uint8_t DLC_Code;
    uint8_t messageContents[8];
}my_can_packet_t;

typedef struct{
    my_can_packet_t buf[CAN_BUFFER_PACKET_LENGTH];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
}my_can_buffer_t;

typedef struct 
{
#ifndef StaticBuffers
    uint8_t *buf;
#else
    uint8_t buf[BufferSize];
#endif
    int size;
    int head;
    int tail;
    int count;    
}RingBuffer_t;
/*
 * Buffer Initialize Function
 * 
 * bool initRingBuffer(RingBuffer_t *newBuff, uint8_t *ptr, int size);
 *
 * returns: Condition on whether the buffer was properly initialized
 * Failed?...
 *  - Verify that the array passed has length > 0 and that the pointer isn't null
 * 
 * Example:
 
    unsigned char myArray[50];
    RingBuffer_t MyBuffer;
    if(!initRingBuffer(&MyBuffer, myArray))
        return;         //Failed to initialize buffer!
 
 */
#ifdef StaticBuffers
//bool initRingBuffer(RingBuffer_t *newBuff, uint8_t *ptr,int size);
#else
RingBuffer_t* createRingBuffer(RingBuffer_t* rBuff, unsigned int s);
#endif
void Buffer_Put(RingBuffer_t *_this, const uint8_t c);
uint8_t Buffer_Get(RingBuffer_t* _this);
int Buffer_Size(RingBuffer_t* _this);
uint8_t Buffer_Peek(RingBuffer_t* _this);
void Buffer_Wipe(RingBuffer_t *ringBuffer);


void CANwipeBuffer(my_can_buffer_t * canBuf);
void CANbufPut(my_can_buffer_t * canBuf, my_can_packet_t p);
my_can_packet_t CANbufGet(my_can_buffer_t * canBuf);
unsigned int buff_get_tail_index(my_can_buffer_t * canBuf);
unsigned int buff_get_head_index(my_can_buffer_t * canBuf);
unsigned int buff_get_last_index(my_can_buffer_t * canBuf);

#endif	/* BUFFERHANDLER_H */
//#endif	/* BUFFERHANDLER_H */

