#include "uart_Handler.h"

unsigned int UART_buff_modulo_inc(const unsigned int value, const unsigned int modulus);
void RXEventHandler(const SYS_MODULE_INDEX index);
void TXEventHandler(const SYS_MODULE_INDEX index);

volatile bool Transmit_stall[6];

unsigned char transmitArrays[6][UART_BUFFER_SIZE];
unsigned char receiveArrays[6][UART_BUFFER_SIZE];

UART_Object_t *_TXEventHandlerUARTobjects[6];
UART_Object_t *_RXEventHandlerUARTobjects[6];
/*  
 * NOTE:
 * If you have separate uart modules for the same uart object call the InitRXUARTModule & InitRTXUARTModule
 * opposded to InitUARTModule below
 */
bool InitUARTModule(UART_Object_t* _UART, SYS_MODULE_INDEX _uartModule)
{
    _UART->RxBuffer = createRingBuffer(_UART->RxBuffer,UART_BUFFER_SIZE);
    _UART->TxBuffer = createRingBuffer(_UART->TxBuffer,UART_BUFFER_SIZE);

    //making the TX&RX modules and handlers all the same
    _UART->uartTXModule = _uartModule;
    _UART->uartRXModule = _uartModule;
    
    _UART->uartTXHandle = DRV_USART_Open(_uartModule, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_SHARED );
    _UART->uartRXHandle = _UART->uartTXHandle;
    
    InitRXEventHandler(_UART);
    InitTXEventHandler(_UART);
    
    if (DRV_HANDLE_INVALID == _UART->uartTXHandle && DRV_HANDLE_INVALID == _UART->uartRXHandle)
    {
        //TODO: make error indication with LEDs (flash them all or something)
        
        // Unable to open the driver
        // May be the driver is not initialized or the initialization
        // is not complete.
        return false;
    }
    return true;
}

bool InitRXUARTModule(UART_Object_t* _UART, SYS_MODULE_INDEX _uartModule)
{
    //setting the ringbuffer's buffer array to an array with predetermined size
    //setting the uart object to reference the buffer instance
    _UART->RxBuffer = createRingBuffer(_UART->RxBuffer,UART_BUFFER_SIZE);
    //storing the uart instance (0 through 6) as possible values
    _UART->uartRXModule = _uartModule;
    //opening the uart channel
    _UART->uartRXHandle = DRV_USART_Open(_uartModule, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_SHARED );
 
    InitRXEventHandler(_UART);
    
    if (DRV_HANDLE_INVALID == _UART->uartRXHandle)
    {
        // Unable to open the driver
        // May be the driver is not initialized or the initialization
        // is not complete.
        return false;
    }
    return true;
}
/* Function: InitTXUARTModule:
 * 
 * Purpose: Sets the functions that will be called when the corresponding interrupt fires
 */
bool InitTXUARTModule(UART_Object_t* _UART, SYS_MODULE_INDEX _uartModule)
{

    _UART->TxBuffer = createRingBuffer(_UART->TxBuffer,UART_BUFFER_SIZE);;
    
    _UART->uartTXModule = _uartModule;
    _UART->uartTXHandle = DRV_USART_Open(_uartModule, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_SHARED );
    InitTXEventHandler(_UART);
    
    if (DRV_HANDLE_INVALID == _UART->uartTXHandle)
    {
        // Unable to open the driver
        // May be the driver is not initialized or the initialization
        // is not complete.
        return false;
    }
    return true;
}
/* Function: InitRXEventHandler & InitTXEventHandler:
 * 
 * Purpose: Sets the functions that will be called when the receive interrupt fires
 */
void InitRXEventHandler(UART_Object_t* _uartObj)
{
    _RXEventHandlerUARTobjects[_uartObj->uartRXModule] = _uartObj;
   
    DRV_USART_ByteReceiveCallbackSet(_uartObj->uartRXModule, RXEventHandler);
}
/* Function: InitTXEventHandler:
 * 
 * Purpose: Sets the functions that will be called when the transmit interrupt fires
 */
void InitTXEventHandler(UART_Object_t* _uartObj)
{
    _TXEventHandlerUARTobjects[_uartObj->uartTXModule] = _uartObj;
    
    DRV_USART_ByteTransmitCallbackSet(_uartObj->uartTXModule, TXEventHandler);
}



/**********************************************************************************
 *********************Transmit/Receive Management Functions************************
 **********************************************************************************/

//Sends the data in the uartObjects TX buffer (thus the buffer must have info to send)
void Send_put(UART_Object_t* _TXobject, unsigned char _data)
{
   
    //Put the data into the RX buffer
    Buffer_Put(_TXobject->TxBuffer, _data);
    //use DRV_USART_TransmitBufferIsFull(Handle);
    //If the interrupt is disabled then we won't call the write function again 
    if( !(DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART_TransferStatus(_TXobject->uartTXHandle)) )
    {
        DRV_USART_WriteByte(_TXobject->uartTXHandle, Buffer_Get(_TXobject->TxBuffer));
    }
}

// Event Processing Technique. Event is received when
// a byte is received.

void RXEventHandler(const SYS_MODULE_INDEX index)
{
    //Byte has been Received. Handle the event.
    DRV_HANDLE  handle = _RXEventHandlerUARTobjects[index]->uartRXHandle;
    while((DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT & DRV_USART_TransferStatus(handle)))
    {
        uint8_t data = DRV_USART_ReadByte(handle);
        Buffer_Put(_RXEventHandlerUARTobjects[index]->RxBuffer ,data);
    } 
}

void TXEventHandler(const SYS_MODULE_INDEX index)
{
    //Byte has finished transmitting
    if(Buffer_Size(_TXEventHandlerUARTobjects[index]->TxBuffer) > 0)
    {
        //Transmit the next byte
        DRV_USART_WriteByte(
                _TXEventHandlerUARTobjects[index]->uartTXHandle,
                Buffer_Get(_TXEventHandlerUARTobjects[index]->TxBuffer));
    }
    else
    {
        Transmit_stall[index] = false;
        //May need to disable the interrupt here!
    }
}


int Receive_available(UART_Object_t* _this)
{
    return Buffer_Size(_this->RxBuffer);
}
//this is need to use the prinf function
void _mon_putc(char c)
{
    while(U2STAbits.UTXBF == 1)
    {
        
    }
    U2TXREG = c;    
}

