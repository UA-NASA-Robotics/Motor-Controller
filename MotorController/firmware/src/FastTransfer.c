#include "FastTransfer.h"

#define FT_START_BYTE_1 0x06
#define FT_START_BYTE_2 0x85

#define polynomial 0x8C  //polynomial used to calculate crc
#define CRC_COUNT 5 // how many AKNAKs are stored
#define CRC_DEPTH 3  // how many pieces of data are stored with each CRC send event
#define CRC_BUFFER_SIZE (CRC_COUNT * CRC_DEPTH) //crc buffer size 5 deep and 3 bytes an entry
//*****************Receive******************************************************
unsigned char rx_buffer[255]; //address for temporary storage and parsing buffer
unsigned char rx_array_inx; //index for RX parsing buffer
unsigned char rx_len = 0; //RX packet length according to the packet
unsigned char calc_CS; //calculated Checksum
unsigned char moduleAddress; // the address of this module
unsigned char returnAddress; //the address to send the crc back to
unsigned char maxDataAddress; //max address allowable
int * receiveArrayAddress; // this is where the data will go when it is received
unsigned char * sendStructAddress; // this is where the data will be sent from
bool AKNAKsend; // turns the acknowledged or not acknowledged on/off
unsigned int alignErrorCounter; //counts the align errors
unsigned int crcErrorCounter; // counts any failed crcs
unsigned int addressErrorCounter; // counts every time a wrong address is received
unsigned int dataAdressErrorCounter; // counts if the received data fall outside of the receive array
unsigned char rx_address; //RX address received

struct crcBufS
{ // this is where the address where sent to, the sent crc, the status of the AKNAK
    unsigned char buf[CRC_BUFFER_SIZE];
    int head;
};
struct crcBufS crc_buffer;

unsigned char crcBufS_get(struct crcBufS* _this, unsigned char time, unsigned char space);
unsigned int FastTransfer_buffer_modulo_inc(const unsigned int value, const unsigned int modulus);
void FastTransfer_buffer_put(FastTransfer_t *_this, unsigned char towhere, unsigned int towhat);
unsigned char CRC8(const unsigned char * data, unsigned char len);
unsigned char _peek(FastTransfer_t *_FTobject);
int _available(FastTransfer_t *_FTobject);
unsigned char _get(FastTransfer_t* _FTobject);
unsigned char CRC8(const unsigned char * data, unsigned char len);
unsigned char AKNAK(unsigned char module);
unsigned int alignError(void);
unsigned int CRCError(void);
unsigned int addressError(void);
unsigned int dataAddressError(void);
void crcBufS_put(struct crcBufS* _this, unsigned char address, unsigned char oldCRC, unsigned char status);
void crcBufS_status_put(struct crcBufS* _this, unsigned char time, unsigned char status);
void CRCcheck(FastTransfer_t * _this);


void InitFastTransferModule(FastTransfer_t* _this,SYS_MODULE_INDEX _uartModule, int _myAddress,  void (*_writeData)(UART_Object_t* _TXobject, unsigned char _data), unsigned char (*_readData)(RingBuffer_t*),int (*_isAvailable)(RingBuffer_t*), unsigned char (*_dataPeek)(RingBuffer_t*))
{
    
    _this->COMdriver.uartTXModule   = _uartModule;
    _this->COMdriver.uartRXModule   = _uartModule;
    _this->ModuleADDRESS            = _myAddress;
    
    _this->serial_write             = _writeData;
    _this->serial_read              = _readData;
    _this->serial_available         = _isAvailable;
    _this->serial_peek              = _dataPeek;
   
    _this->DataBuffer = createRingBuffer(_this->DataBuffer, BUFFER_SIZE);
   
    //this is to keep FT library similar to Legacy library
    maxDataAddress                  = sizeof(_this->ReceivedData);
    receiveArrayAddress             = _this->ReceivedData;
    
    //LOOKOUT
    _this->ReceivedData[8]=1;
   
    InitUARTModule(&_this->COMdriver, _uartModule);
}
void CloseFastTransferModule(FastTransfer_t* _this)
{
    if(_this->COMdriver.uartTXHandle == _this->COMdriver.uartRXHandle)
    {
        //doesn't have to be the tx handle i just randomly pick on since they are the same
        DRV_USART_Close(_this->COMdriver.uartTXHandle);
    }
    else
    {
        DRV_USART_Close(_this->COMdriver.uartTXHandle);
        DRV_USART_Close(_this->COMdriver.uartRXHandle);
    }
}

bool isFTFlagSet(FastTransfer_t *_this, unsigned char index)
{
    return _this->ReceivedDataFlags[index];
}
int16_t getFTReceivedData(FastTransfer_t *_this, unsigned char index)
{
    _this->ReceivedDataFlags[index] = false;
    return _this->ReceivedData[index];
}

void ToSend(FastTransfer_t *_this,short _address, int _data)
{
    FastTransfer_buffer_put(_this, _address, _data);
}

bool sendData(FastTransfer_t *_FTobject, unsigned char whereToSend)
{

    //calculate the crc
    unsigned char CS = CRC8(_FTobject->DataBuffer->buf, _FTobject->DataBuffer->count);

    _FTobject->serial_write(&_FTobject->COMdriver,FT_START_BYTE_1); //start address
    _FTobject->serial_write(&_FTobject->COMdriver,FT_START_BYTE_2); //start address
    _FTobject->serial_write(&_FTobject->COMdriver,whereToSend);
    _FTobject->serial_write(&_FTobject->COMdriver,_FTobject->ModuleADDRESS);
    _FTobject->serial_write(&_FTobject->COMdriver,_FTobject->DataBuffer->count); //length of packet not including the crc


    //send the rest of the packet
    int i;
    for (i = 0; i < _FTobject->DataBuffer->count; i++)
    {
        _FTobject->serial_write(&_FTobject->COMdriver,*(_FTobject->DataBuffer->buf + i));
    }

    //send the crc
    _FTobject->serial_write(&_FTobject->COMdriver, CS);

    //record the sent message data for aknak check later
    //crcBufS_put(&crc_buffer, whereToSend, CS, 0);
    
    // clears the buffer after a sending
    Buffer_Wipe(_FTobject->DataBuffer);
    return true;
}

 void FastTransfer_buffer_put(FastTransfer_t *_this, unsigned char towhere, unsigned int towhat)
{

    group.integer = towhat;

    if (_this->DataBuffer->count < (BUFFER_SIZE - 3))
    {
        _this->DataBuffer->buf[_this->DataBuffer->head] = towhere;
        _this->DataBuffer->head = FastTransfer_buffer_modulo_inc(_this->DataBuffer->head, BUFFER_SIZE);
        ++_this->DataBuffer->count;
        _this->DataBuffer->buf[_this->DataBuffer->head] = group.parts[0];
        _this->DataBuffer->head = FastTransfer_buffer_modulo_inc(_this->DataBuffer->head, BUFFER_SIZE);
        ++_this->DataBuffer->count;
        _this->DataBuffer->buf[_this->DataBuffer->head] = group.parts[1];
        _this->DataBuffer->head = FastTransfer_buffer_modulo_inc(_this->DataBuffer->head, BUFFER_SIZE);
        ++_this->DataBuffer->count;

    }

}
 unsigned int FastTransfer_buffer_modulo_inc(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = value + 1;
    if (my_value >= modulus)
    {
        my_value = 0;
    }
    return (my_value);
}

bool receiveData(FastTransfer_t *_this)
{
  

    //start off by looking for the header bytes. If they were already found in a previous call, skip it.
    if (rx_len == 0)
    {
        //this size check may be redundant due to the size check below, but for now I'll leave it the way it is.
        if (_this->serial_available(_this->COMdriver.RxBuffer) > 5)
        {
            
            //this will block until a 0x06 is found or buffer size becomes less then 3.
            while (_this->serial_read(_this->COMdriver.RxBuffer) != 0x06)
            {
                //This will trash any preamble junk in the serial buffer
                //but we need to make sure there is enough in the buffer to process while we trash the rest
                //if the buffer becomes too empty, we will escape and try again on the next call
                alignErrorCounter++; //increments the counter whenever a byte is trashed
                if (_this->serial_available(_this->COMdriver.RxBuffer) < 5)
                    return false;
            }
            if (_this->serial_read(_this->COMdriver.RxBuffer) == 0x85)
            {
                rx_address = _this->serial_read(_this->COMdriver.RxBuffer); // pulls the address
                returnAddress = _this->serial_read(_this->COMdriver.RxBuffer); // pulls where the message came from
                rx_len = _this->serial_read(_this->COMdriver.RxBuffer); // pulls the length
               
                               
                //make sure the address received is a match for this module if not throw the packet away
              
                if (rx_address != _this->ModuleADDRESS)
                {
                    addressErrorCounter++; // increments a counter whenever the wrong address is received
                    //if the address does not match the buffer is flushed for the size of
                    //the data packet plus one for the CRC
                    int u;
                    for (u = 0; u <= (rx_len + 1); u++)
                    {
                        _this->serial_read(_this->COMdriver.RxBuffer);
                    }
                    rx_len = 0; // reset length
                    return false;
                }
                // if the address matches the a dynamic buffer is created to store the received data
                //rx_buffer = (unsigned char*) malloc(rx_len + 1);
            }
        }
    }

    //we get here if we already found the header bytes, the address matched what we know, and now we are byte aligned.
    if (rx_len != 0)
    {
        //DRV_USART0_WriteByte(0xFC);
        //this check is preformed to see if the first data address is a 255, if it is then this packet is an AKNAK
        if (rx_array_inx == 0)
        {
            while (!(_this->serial_available(_this->COMdriver.RxBuffer) >= 1));
            if (255 == _this->serial_peek(_this->COMdriver.RxBuffer))
            {
                CRCcheck(_this);
                rx_len = 0;
                rx_array_inx = 0;
                //free(rx_buffer);
                
                //Buffer_Wipe(rx_buffer,255);
                return receiveData(_this);
            }
        }

        
        while ((_this->serial_available(_this->COMdriver.RxBuffer)) > 0 && ( rx_array_inx <= rx_len))//(serial_available() > 0) &&( rx_array_inx <= rx_len))
        {  
             rx_buffer[rx_array_inx++] = _this->serial_read(_this->COMdriver.RxBuffer);
              
        }

        if (rx_len == (rx_array_inx - 1))
        {
            //seem to have got whole message
            //last uint8_t is CS
            calc_CS = CRC8(rx_buffer, rx_len);


            
            if (calc_CS == rx_buffer[rx_array_inx - 1])
            {//CS good

                // reassembles the data and places it into the receive array according to data address.
                int r;
                for (r = 0; r < rx_len; r = r + 3)
                {
                    if (rx_buffer[r] < maxDataAddress)
                    {
                        group.parts[0] = rx_buffer[r + 1];
                        group.parts[1] = rx_buffer[r + 2];
                        
                        _this->ReceivedData[(rx_buffer[r])] = group.integer;
                        _this->ReceivedDataFlags[(rx_buffer[r])] = true;
                        
                    } else
                    {
                        dataAdressErrorCounter++;
                    }
                }
//TODO: need to Make this fit the new serial_write parameters
//                if (AKNAKsend)
//                { // if enabled sends an AK
//                    unsigned char holder[3];
//                    holder[0] = 255;
//                    holder[1] = 1;
//                    holder[2] = rx_buffer[rx_array_inx - 1];
//                    unsigned char crcHolder = CRC8(holder, 3);
//                    _this->serial_write(0x06);
//                    _this->serial_write(0x85);
//                    _this->serial_write(returnAddress);
//                    _this->serial_write(moduleAddress);
//                    _this->serial_write(3);
//                    _this->serial_write(255);
//                    _this->serial_write(1);
//                    _this->serial_write(rx_buffer[rx_array_inx - 1]);
//                    _this->serial_write(crcHolder);
//                }


                
                rx_len = 0;
                rx_array_inx = 0;
                //free(rx_buffer);
               
                //Buffer_Wipe(rx_buffer,255);
                return true;
            }
            else
            {
                crcErrorCounter++; //increments the counter every time a crc fails

//TODO: need to Make this fit the new serial_write parameters
//                if (AKNAKsend)
//                { // if enabled sends NAK
//                    unsigned char holder[3];
//                    holder[0] = 255;
//                    holder[1] = 2;
//                    holder[2] = rx_buffer[rx_array_inx - 1];
//                    unsigned char crcHolder = CRC8(holder, 3);
//                    serial_write(0x06);
//                    serial_write(0x85);
//                    serial_write(returnAddress);
//                    serial_write(moduleAddress);
//                    serial_write(3);
//                    serial_write(255);
//                    serial_write(2);
//                    serial_write(rx_buffer[rx_array_inx - 1]);
//                    serial_write(crcHolder);
//                }
                
                //failed checksum, need to clear this out
                rx_len = 0;
                rx_array_inx = 0;
                //free(rx_buffer);
                
               // Buffer_Wipe(rx_buffer,255);
                return false;
            }
        }
    }
    return false;
}
/*******************************************************************
 * Use the #define functions to access the data in the Legacy method
 *******************************************************************
 */

//#define Receive_peak(_FT)       _peek(_FT)
//#define Receive_available(_FT)  _available(_FT)
//#define Receive_get(_FT)        _get(_FT)

/*******************************************************************
 * Don't use the functions below to access FastTransfer data!!!!
 ******************************************************************/
unsigned char _peek(FastTransfer_t* _FTobject)
{
    return _FTobject->serial_peek(_FTobject->COMdriver.RxBuffer);
}

int _available(FastTransfer_t *_FTobject)
{
    return _FTobject->serial_available(_FTobject->COMdriver.RxBuffer);
}
unsigned char _get(FastTransfer_t* _FTobject)
{
    return _FTobject->serial_read(_FTobject->COMdriver.RxBuffer);
}
/*******************************************************************
 ******************************************************************/

unsigned char CRC8(const unsigned char * data, unsigned char len)
{
    unsigned char crc = 0x00;
    while (len--)
    {
        unsigned char extract = *data++;
        unsigned char tempI;
        for (tempI = 8; tempI; tempI--)
        {
            unsigned char sum = (crc ^ extract) & 0x01;
            crc >>= 1;
            if (sum)
            {
                crc ^= polynomial;
            }
            extract >>= 1;
        }
    }
    return crc;
}


//searches the buffer for the status of a message that was sent

unsigned char AKNAK(unsigned char module)
{
    int r;
    for (r = 0; r < CRC_COUNT; r++)
    {
        if (module == crcBufS_get(&crc_buffer, r, 0))
        {
            return crcBufS_get(&crc_buffer, r, 2);
        }
    }
    return 4;
}


//returns align error

unsigned int alignError(void)
{
    return alignErrorCounter;
}


//returns CRC error

unsigned int CRCError(void)
{
    return crcErrorCounter;
}


//returns address error

unsigned int addressError(void)
{
    return addressErrorCounter;
}

unsigned int dataAddressError(void)
{
    return dataAdressErrorCounter;
}

// after a packet is sent records the info of that packet

void crcBufS_put(struct crcBufS* _this, unsigned char address, unsigned char oldCRC, unsigned char status)
{
    _this->buf[_this->head] = address;
    _this->head++;
    _this->buf[_this->head] = oldCRC;
    _this->head++;
    _this->buf[_this->head] = status;
    _this->head++;
    if (_this->head >= CRC_BUFFER_SIZE)
    {
        _this->head = 0;
    }
}


// after a Ak or NAK is received that status is stored
void crcBufS_status_put(struct crcBufS* _this, unsigned char time, unsigned char status)
{

    if (time >= CRC_COUNT)
    {
        time = CRC_COUNT - 1;
    }
    time = time + 1;
    int wantedTime = time * 3;
    if (wantedTime > _this->head)
    {
        wantedTime = (CRC_BUFFER_SIZE) - (wantedTime - _this->head);
        _this->buf[(wantedTime + 2)] = status;
    } else
    {
        _this->buf[(_this->head - wantedTime) + 2] = status;
    }
}


// pulls data from the AKNAK buffer

unsigned char crcBufS_get(struct crcBufS* _this, unsigned char time, unsigned char space)
{
    if (time >= CRC_COUNT)
    {
        time = CRC_COUNT - 1;
    }
    if (space >= CRC_DEPTH)
    {
        space = CRC_DEPTH - 1;
    }
    time = time + 1;
    int wantedTime = time * 3;
    if (wantedTime > _this->head)
    {
        wantedTime = (CRC_BUFFER_SIZE) - (wantedTime - _this->head);
        return (_this->buf[(wantedTime + space)]);
    } else
    {
        return (_this->buf[(_this->head - wantedTime) + space]);
    }
}


//when an AK or NAK is received this compares it to the buffer and records the status

void CRCcheck(FastTransfer_t *_this)
{

    while (!(_this->serial_available(_this->COMdriver.RxBuffer) > 3)); // trap makes sure that there are enough bytes in the buffer for the AKNAK check

    unsigned char arrayHolder[3];
    arrayHolder[0] = _this->serial_read(_this->COMdriver.RxBuffer);
    arrayHolder[1] = _this->serial_read(_this->COMdriver.RxBuffer);
    arrayHolder[2] = _this->serial_read(_this->COMdriver.RxBuffer);
    unsigned char SentCRC = _this->serial_read(_this->COMdriver.RxBuffer);
    unsigned char calculatedCRC = CRC8(arrayHolder, 3);


    if (SentCRC == calculatedCRC)
    {

        int rt;
        for (rt = 0; rt < CRC_COUNT; rt++)
        {
            if (returnAddress == crcBufS_get(&crc_buffer, rt, 0))
            {
                if (arrayHolder[2] == crcBufS_get(&crc_buffer, rt, 1))
                {
                    if (arrayHolder[1] == 1)
                    {
                        crcBufS_status_put(&crc_buffer, rt, 1);
                        break;
                    } else if (arrayHolder[1] == 2)
                    {
                        crcBufS_status_put(&crc_buffer, rt, 2);
                        break;
                    }
                }
            }
        }
    } else
    {
        crcErrorCounter++;
    } //increments the counter every time a crc fails
}


