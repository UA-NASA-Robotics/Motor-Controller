
#include "CANbufferHandler.h"
#include <stdint.h>


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
