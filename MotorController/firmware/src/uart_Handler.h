/* 
 * File:   uart_Handler.h
 * Author: John
 *
 * Created on February 28, 2018, 7:59 PM
 */

#ifndef UART_HANDLER_H
#define	UART_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/usart/drv_usart.h"
#include "bufferHandler.h"

#define UART_Master       DRV_USART_INDEX_0       //Uart2(Silicon instance) | Instance 0 (harmony) | UART2_3TX(tx) UART2_3RX(rx) (board Schematic) | PIN 53(TX) & PIN 52(RX) (Processor)
#define UART_Gyro         DRV_USART_INDEX_1       //Uart4(Silicon instance) | Instance 1 (harmony) | UART1_2RX(tx) UART1_2TX(rx) (board Schematic) | PIN 52(TX) & PIN 53(RX) (Processor)
#define Uart_3            DRV_USART_INDEX_2       //Uartx(Silicon instance) | Instance 2 (harmony) | <title> (board Schematic) | PIN xx(TX) & PIN xx(RX) (Processor)
#define Uart_4            DRV_USART_INDEX_3       //Uartx(Silicon instance) | Instance 3 (harmony) | <title> (board Schematic) | PIN xx(TX) & PIN xx(RX) (Processor)
#define Uart_5            DRV_USART_INDEX_4       //Uartx(Silicon instance) | Instance 4 (harmony) | <title> (board Schematic) | PIN xx(TX) & PIN xx(RX) (Processor)
#define Uart_6            DRV_USART_INDEX_5       //Uartx(Silicon instance) | Instance x (harmony) | <title> (board Schematic) | PIN xx(TX) & PIN xx(RX) (Processor)
    


//Ring Buffer parameters for input UART (UART 4: U4RX)
#define UART_BUFFER_SIZE 500 
typedef enum
{
    MODULE_ERROR
}ERROR_STATES;



typedef struct
{
    
    /*Receive buffer*/
    RingBuffer_t *RxBuffer;
    /*Transmit buffer*/
    RingBuffer_t *TxBuffer;
    /*The status of the object*/
    ERROR_STATES currentState;
    /* Client Status */
    DRV_USART_CLIENT_STATUS usartStatus;
    
    /***********************************************************************************************/
    /* (uartRXHandle & uartTXHandle) and (uartRXModule & uartTXModule) will be set to the same value
     * as uartHandle unless otherwise set using InitRXUARTModule and
     * InitTXUARTModule which will assign different handers and modules
     * to RX and TX */
    /* USART driver handle */
    DRV_HANDLE uartRXHandle;
    DRV_HANDLE uartTXHandle;

    SYS_MODULE_INDEX uartRXModule;
    SYS_MODULE_INDEX uartTXModule;
    /***********************************************************************************************/
    
    /* Handle returned by USART for buffer submitted */
    DRV_USART_BUFFER_HANDLE usartBufferHandle;

    /* Handle returned by USART for buffer event */
    DRV_USART_BUFFER_EVENT  usartBufferEvent;

    /* Flag to indicate the driver message is been processed */
    bool drvBufferEventComplete;
    
}UART_Object_t;
/*
 * Initialization of UART objects HERE
 */

    UART_Object_t MasterUart;
    UART_Object_t GyroUart;
    

int Receive_available(UART_Object_t* _this);
/************************************************************************************
 ************************************************************************************/

void Send_put(UART_Object_t* _TXobject, unsigned char _data);

bool InitUARTModule(UART_Object_t* _UART, SYS_MODULE_INDEX _uartModule);

void InitRXEventHandler(UART_Object_t* _uartObj);
void InitTXEventHandler(UART_Object_t* _uartObj);
void myprintf(UART_Object_t* _TXobject, char * stringed);

#endif	/* UART_HANDLER_H */
