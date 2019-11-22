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
#define BUFFER_SIZE_CAN 200
typedef struct {
    int buf[BUFFER_SIZE_CAN];
    int head;
    int tail;
    int count;
} ringBufCAN_t;

void buffer_put_3(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat, unsigned int val);
unsigned int buffer_get(ringBufCAN_t* _this);
unsigned int buffer_peek(ringBufCAN_t* _this);
void buffer_flush(ringBufCAN_t* _this, const int clearBuffer);
void wipeBuffer(int*buf, int val, uint16_t numVals);
unsigned int buffer_modulo_inc(const unsigned int value, const unsigned int modulus);
unsigned int buffer_GetCount(ringBufCAN_t* _this);


#ifdef	__cplusplus
}
#endif

#endif	/* CANBUFFERHANDLER_H */

