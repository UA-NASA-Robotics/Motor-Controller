#include "bufferHandler.h"


unsigned int buff_modulo_inc(const unsigned int value, const unsigned int modulus);

#ifdef StaticBuffers
bool initRingBuffer(RingBuffer_t *newBuff, uint8_t *ptr, int size)
{
    
    if(ptr == 0)
        return false;
    newBuff->buf = ptr;      //setting the pointer to the passed in array to be the buffer array 
    
    
    newBuff->size = size;
    if(newBuff->size == 0)
        return 0;
   
    Buffer_Wipe(newBuff);
    return true;
}
#else
RingBuffer_t* createRingBuffer(RingBuffer_t* rBuff, unsigned int s)
{
    //RingBuffer_t* rBuff = malloc(sizeof(RingBuffer_t) +s*sizeof(uint8_t*));
    rBuff = malloc(sizeof(RingBuffer_t));
    rBuff->buf = malloc(sizeof(uint8_t) * s);
    
    if(!rBuff){   
        //printf("Failure to malloc Buffer of size %u + %u", (unsigned int)(sizeof(RingBuffer_t*)), (unsigned int)(s*sizeof(uint8_t*)));
        return NULL;
    }
    rBuff->size = s;
    Buffer_Wipe(rBuff);
    
    return rBuff;
}

#endif
void Buffer_Wipe(RingBuffer_t *ringBuffer)
{
    ringBuffer->head = 0;
    ringBuffer->tail = 0;
    ringBuffer->count = 0;
    int i;
    for(i=0;i<ringBuffer->size;i++)
    {
        ringBuffer->buf[i]=0;
    }
}
void Buffer_Put(RingBuffer_t *_this, const uint8_t c)
{
    const int BUFFER_SIZE = _this->size;
    if (_this->count < BUFFER_SIZE)
    {
        _this->buf[_this->head] = c;
        _this->head = buff_modulo_inc(_this->head, BUFFER_SIZE);
        _this->count++;
        
    } else
    {
        _this->buf[_this->head] = c;
        _this->head = buff_modulo_inc(_this->head, BUFFER_SIZE);
        _this->tail = buff_modulo_inc(_this->tail, BUFFER_SIZE);

    }
}
uint8_t Buffer_Get(RingBuffer_t* _this)
{
    const int BUFFER_SIZE = _this->size;
    uint8_t c;
    if (_this->count > 0)
    {
        c = _this->buf[_this->tail];
        _this->tail = buff_modulo_inc(_this->tail, BUFFER_SIZE);
        --_this->count;
    }else{
        c = 0;
    }
    return (c);
}
int Buffer_Size(RingBuffer_t* _this)
{
    return _this->count;
}

unsigned int buff_modulo_inc(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = value + 1;
    if (my_value >= modulus)
    {
        my_value = 0;
    }
    return (my_value);
}
uint8_t Buffer_Peek(RingBuffer_t* _this)
{
    return _this->buf[_this->tail];
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
        canBuf->head = buff_modulo_inc(canBuf->head, CAN_BUFFER_PACKET_LENGTH);
        ++canBuf->count;
    } 
     else
    {
        canBuf->buf[canBuf->head] = p;
        canBuf->head = buff_modulo_inc(canBuf->head, CAN_BUFFER_PACKET_LENGTH);
        canBuf->tail = buff_modulo_inc(canBuf->tail, CAN_BUFFER_PACKET_LENGTH);

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
        canBuf->tail = buff_modulo_inc(canBuf->tail, CAN_BUFFER_PACKET_LENGTH);
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