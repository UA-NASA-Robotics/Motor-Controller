#include "CANFastTransfer.h"


#define BUFFER_SIZE_CAN 200
                            //0b0111 1111 11xx xxxx (where the x's are your address)
#define LAST_BOARD_RECEIEVED 0
typedef struct
{
    int buf[BUFFER_SIZE_CAN];
	int head;
	int tail;
	int count;    
}ringBufCAN_t;

void beginCANFast(volatile int * ptr, unsigned int maxSize, unsigned char givenAddress);
void buffer_put(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat);
void buffer_put_3(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat, unsigned int val);
unsigned int buffer_get(ringBufCAN_t* _this);
unsigned int buffer_peek(ringBufCAN_t* _this); 
void buffer_flush(ringBufCAN_t* _this, const int clearBuffer);
void wipeBuffer(int*buf, int val, uint16_t numVals);
unsigned int buffer_modulo_inc(const unsigned int value, const unsigned int modulus);
unsigned int buffer_GetCount(ringBufCAN_t* _this);

ringBufCAN_t transmit_buffer_CAN,send_buffer_CAN_FT, rx_buffer_CAN;
bool dataReceived= false;
volatile int * receiveArrayAddressCAN;
unsigned char moduleAddressCAN;
unsigned int MaxIndex;
int receiveArrayCAN[10];

int * getReceiveArrayCAN(void)
{
    return receiveArrayCAN;
}

void initCANFT(void)
{    
    beginCANFast(receiveArrayCAN, sizeof(receiveArrayCAN), MY_ADDRESS);
}


void beginCANFast(volatile int * ptr, unsigned int maxSize, unsigned char givenAddress)
{
	receiveArrayAddressCAN = ptr;
	moduleAddressCAN = givenAddress;
	MaxIndex = maxSize;
	
	buffer_flush(&rx_buffer_CAN,0);
	buffer_flush(&send_buffer_CAN_FT,0);
	buffer_flush(&transmit_buffer_CAN,0); 
}
int getCANFastData(uint8_t index)
{
    ReceiveDataCAN();
     if(index < MaxIndex)
     {
         return receiveArrayCAN[index];
     }
    return 0xFFFF;
}
void ReceiveCANFast( my_can_packet_t *p) // interrupt callback
{
    if((p->canAddress >> 6) == moduleAddressCAN)
    {
        if(p->DLC_Code == 8 || p->DLC_Code == 9) //Check number of bytes, if 8 read in two ints
        {
            if((p->messageContents[0]<<8) +(p->messageContents[1]) < MaxIndex) {
                buffer_put(&rx_buffer_CAN, (p->messageContents[0]<<8) +(p->messageContents[1]), (p->messageContents[2]<<8) +(p->messageContents[3]));
                dataReceived = 1;
            }
            if((p->messageContents[4]<<8) +(p->messageContents[5]) < MaxIndex) {
                buffer_put(&rx_buffer_CAN, (p->messageContents[4]<<8) +(p->messageContents[5]), (p->messageContents[6]<<8) +(p->messageContents[7]));
                dataReceived = 1;
            }
            buffer_put(&rx_buffer_CAN, LAST_BOARD_RECEIEVED, (p->canAddress & 0b11111));
        }
        else //else read in one int
        {
            if((p->messageContents[0]<<8) +(p->messageContents[1]) < MaxIndex) {
                buffer_put(&rx_buffer_CAN, (p->messageContents[0]<<8) +(p->messageContents[1]), (p->messageContents[2]<<8) +(p->messageContents[3]));
                dataReceived = 1; 
            }
            buffer_put(&rx_buffer_CAN, LAST_BOARD_RECEIEVED, (p->canAddress & 0b11111));
        }
    }
}


int ReceiveDataCAN(void) 
{
	if(dataReceived) {
		dataReceived = 0;
		int i = buffer_GetCount(&rx_buffer_CAN);
		if(i) //this better be true ... if not in instant receive 
		{
			for(;i>0; i=i-2)
			{
				int address = buffer_get(&rx_buffer_CAN);
				receiveArrayAddressCAN[address] = buffer_get(&rx_buffer_CAN);
			}
			return 1;
		}
		else
		{
			//error (how was ReceiveData true if no data available)
			return 0;
		}
		
	}
	else
	return 0;
}

bool TransmitCANFast( my_can_packet_t *p) // interrupt callback
{
	if(buffer_GetCount(&transmit_buffer_CAN) > 0) 
	{
        
		//if more than 2 data/index pairs left might be able to send large packet. 
		if(buffer_GetCount(&transmit_buffer_CAN)>6) {
			unsigned int address = buffer_get(&transmit_buffer_CAN);
			p->canAddress = ( address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
			//we are good to send the first index/value pair for sure. 
            int i=0;
			for(i = 0; i<2; i++) 
            {
				unsigned int temp = buffer_get(&transmit_buffer_CAN);
				p->messageContents[2*i] = (temp >> 8);
				p->messageContents[2*i + 1] = temp;
			}
			//now we need to check that the next address matches the current address
			if (buffer_peek(&transmit_buffer_CAN) == address) 
            {
				//address match and we are clear to send them in a single packet
				buffer_get(&transmit_buffer_CAN); //call this to clear out the address
                int i=0;
				for(i = 2; i<4; i++) {
					unsigned int temp = buffer_get(&transmit_buffer_CAN);
					p->messageContents[2*i] = (temp >> 8);
					p->messageContents[2*i + 1] = temp;
				}
				//need to check if this was the last packet 
				if(buffer_peek(&transmit_buffer_CAN) == address) 
                {
					//next address is same, so this isn't the last packet, send a length 8.
					p->DLC_Code = 8;
				}
				else 
                {
					//next address is different, and this is the last packet, so send a 9.
					p->DLC_Code = 9; 
				}
			}
			else 
            {
				//addresses don't match and we should just send what is currently in the packet. 
				p->DLC_Code = 4; 
			}
		}
			//if exactly 2 data/index pairs left send with length 9. Receiver
			//will read the "wrong" length correctly, but realize this is the last packet.
			//note: still need to check incase two different destinations. 
		else if(buffer_GetCount(&transmit_buffer_CAN)==6) 
        {
			unsigned int address = buffer_get(&transmit_buffer_CAN);
			p->canAddress = ( address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
			//we are good to send the first index/value pair for sure.
            int i=0;
			for(i = 0; i<2; i++) 
            {
				unsigned int temp = buffer_get(&transmit_buffer_CAN);
				p->messageContents[2*i] = (temp >> 8);//this is so dumb
				p->messageContents[2*i + 1] = temp;
			}
			//now we need to check that the next address matches the current address
			if (buffer_peek(&transmit_buffer_CAN) == address) 
            {
				//address match and we are clear to send them in a single packet
				buffer_get(&transmit_buffer_CAN); //call this to clear out the address
				int i=0;
                for(i = 2; i<4; i++) 
                {
					unsigned int temp = buffer_get(&transmit_buffer_CAN);
					p->messageContents[2*i] = (temp >> 8);
					p->messageContents[2*i + 1] = temp;
				}
				p->DLC_Code = 9;
			}
			else 
            {
				//addresses don't match and we should just send what is currently in the packet.
				p->DLC_Code = 4;
			}
			
		}
			//if only 1 data/index pair receiver will know it is the last packet.
		else if(buffer_GetCount(&transmit_buffer_CAN)==3)
        {
			unsigned int address = buffer_get(&transmit_buffer_CAN);
			p->canAddress = ( address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
			p->DLC_Code = 4;
            int i=0;
			for(i = 0; i<2; i++) 
            {
				unsigned int temp = buffer_get(&transmit_buffer_CAN);
				p->messageContents[2*i] = (temp >> 8);
				p->messageContents[2*i + 1] = temp;
			}
		}
	
		else 
		{
			//error, missing dest/index/value set, previous packets could be very corrupt
			//TransmitSetMissMatch++;
			buffer_flush(&transmit_buffer_CAN,1); 
            return false;
		}
        return true;
	}
	else 
    {
        C1FIFOINT2bits.TXEMPTYIE = 0;
        return false;
	}
}
    
void ToSendCAN(unsigned int where, unsigned int what)
{
	buffer_put(&send_buffer_CAN_FT, where, what);
}
    
void sendDataCAN( unsigned int whereToSend)
{	
	//NEW STATIC METHOD
	int temp = buffer_GetCount(&send_buffer_CAN_FT); //get size of things to send
	
    int i=0;
	for(i = 0; i<(temp>>1); i++) { //need to divid by two since reading index/value pairs, hence >>1
		int index = buffer_get(&send_buffer_CAN_FT); 
		int value = buffer_get(&send_buffer_CAN_FT); 
		buffer_put_3(&transmit_buffer_CAN, whereToSend, index, value); 
	}
    if(C1FIFOINT2bits.TXEMPTYIE == 0)
    {
        C1FIFOINT2bits.TXEMPTYIE = 1;
    }
}

    
void buffer_put(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat) 
{


	if (_this->count < (BUFFER_SIZE_CAN - 3)) {
		_this->buf[_this->head] = towhere;
		_this->head = buffer_modulo_inc(_this->head, BUFFER_SIZE_CAN);
		++_this->count;
		_this->buf[_this->head] = towhat;
		_this->head = buffer_modulo_inc(_this->head, BUFFER_SIZE_CAN);
		++_this->count;
	}

}
 
void buffer_put_3(ringBufCAN_t *_this, unsigned int towhere, unsigned int towhat, unsigned int val) 
{
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

unsigned int buffer_get(ringBufCAN_t* _this) 
{
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


unsigned int buffer_peek(ringBufCAN_t* _this) 
{
	unsigned int c;
	if (_this->count > 0) {
		c = _this->buf[_this->tail];
	} 
	else {
		c = 0;
	}
	return (c);
}


//flushes the send buffer to get it ready for new data

void buffer_flush(ringBufCAN_t* _this, const int clearBuffer) 
{
	_this->count = 0;
	_this->head = 0;
	_this->tail = 0;
	if (clearBuffer) {
		wipeBuffer(_this->buf, 0, sizeof (_this->buf));
	}
}

void wipeBuffer(int*buf, int val, uint16_t numVals)
{
    int i=0;
    for(i=0;i<numVals;i++)
    {
        buf[i]=val;
    }
}

// increments counters for the buffer functions

unsigned int buffer_modulo_inc(const unsigned int value, const unsigned int modulus) 
{
	unsigned int my_value = value + 1;
	if (my_value >= modulus) {
		my_value = 0;
	}
	return (my_value);
}

//getter for send circular buffer. 
unsigned int buffer_GetCount(ringBufCAN_t* _this) 
{
	return _this->count; 	
}

