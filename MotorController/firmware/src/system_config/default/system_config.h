/*******************************************************************************
  MPLAB Harmony System Configuration Header

  File Name:
    system_config.h

  Summary:
    Build-time configuration header for the system defined by this MPLAB Harmony
    project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    This configuration header must not define any prototypes or data
    definitions (or include any files that do).  It only provides macro
    definitions for build-time configuration options that are not instantiated
    until used by another MPLAB Harmony module or application.

    Created with MPLAB Harmony Version 2.05
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _SYSTEM_CONFIG_H
#define _SYSTEM_CONFIG_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: System Service Configuration
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Common System Service Configuration Options
*/
#define SYS_VERSION_STR           "2.05"
#define SYS_VERSION               20500

// *****************************************************************************
/* Clock System Service Configuration Options
*/
#define SYS_CLK_FREQ                        200000000ul
#define SYS_CLK_BUS_PERIPHERAL_1            100000000ul
#define SYS_CLK_BUS_PERIPHERAL_2            33333333ul
#define SYS_CLK_BUS_PERIPHERAL_3            40000000ul
#define SYS_CLK_BUS_PERIPHERAL_4            100000000ul
#define SYS_CLK_BUS_PERIPHERAL_5            25000000ul
#define SYS_CLK_BUS_PERIPHERAL_7            200000000ul
#define SYS_CLK_BUS_PERIPHERAL_8            66666666ul
#define SYS_CLK_CONFIG_PRIMARY_XTAL         24000000ul
#define SYS_CLK_CONFIG_SECONDARY_XTAL       32768ul
   
/*** Ports System Service Configuration ***/
#define SYS_PORT_B_ANSEL        0x401F
#define SYS_PORT_B_TRIS         0x6BDF
#define SYS_PORT_B_LAT          0x1400
#define SYS_PORT_B_ODC          0x0000
#define SYS_PORT_B_CNPU         0x0000
#define SYS_PORT_B_CNPD         0x0000
#define SYS_PORT_B_CNEN         0x2800

#define SYS_PORT_C_ANSEL        0x9FFF
#define SYS_PORT_C_TRIS         0xFFFF
#define SYS_PORT_C_LAT          0x0000
#define SYS_PORT_C_ODC          0x0000
#define SYS_PORT_C_CNPU         0x0000
#define SYS_PORT_C_CNPD         0x0000
#define SYS_PORT_C_CNEN         0x0000

#define SYS_PORT_D_ANSEL        0xF1C0
#define SYS_PORT_D_TRIS         0xFFFF
#define SYS_PORT_D_LAT          0x0000
#define SYS_PORT_D_ODC          0x0000
#define SYS_PORT_D_CNPU         0x0000
#define SYS_PORT_D_CNPD         0x0000
#define SYS_PORT_D_CNEN         0x0000

#define SYS_PORT_E_ANSEL        0xFF00
#define SYS_PORT_E_TRIS         0xFF03
#define SYS_PORT_E_LAT          0x00EC
#define SYS_PORT_E_ODC          0x0000
#define SYS_PORT_E_CNPU         0x0000
#define SYS_PORT_E_CNPD         0x0000
#define SYS_PORT_E_CNEN         0x0001

#define SYS_PORT_F_ANSEL        0xFFC4
#define SYS_PORT_F_TRIS         0xFFFF
#define SYS_PORT_F_LAT          0x0000
#define SYS_PORT_F_ODC          0x0000
#define SYS_PORT_F_CNPU         0x0000
#define SYS_PORT_F_CNPD         0x0000
#define SYS_PORT_F_CNEN         0x0002

#define SYS_PORT_G_ANSEL        0xFC7F
#define SYS_PORT_G_TRIS         0xFFBF
#define SYS_PORT_G_LAT          0x0040
#define SYS_PORT_G_ODC          0x0000
#define SYS_PORT_G_CNPU         0x0200
#define SYS_PORT_G_CNPD         0x0000
#define SYS_PORT_G_CNEN         0x0000


/*** Interrupt System Service Configuration ***/
#define SYS_INT                     true

// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************
#define DRV_CAN_DRIVER_MODE_STATIC 
/*** Timer Driver Configuration ***/
#define DRV_TMR_INTERRUPT_MODE             true

/*** Timer Driver 0 Configuration ***/
#define DRV_TMR_PERIPHERAL_ID_IDX0          TMR_ID_1
#define DRV_TMR_INTERRUPT_SOURCE_IDX0       INT_SOURCE_TIMER_1
#define DRV_TMR_INTERRUPT_VECTOR_IDX0       INT_VECTOR_T1
#define DRV_TMR_ISR_VECTOR_IDX0             _TIMER_1_VECTOR
#define DRV_TMR_INTERRUPT_PRIORITY_IDX0     INT_PRIORITY_LEVEL6
#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX0 INT_SUBPRIORITY_LEVEL3
#define DRV_TMR_CLOCK_SOURCE_IDX0           DRV_TMR_CLKSOURCE_INTERNAL
#define DRV_TMR_PRESCALE_IDX0               TMR_PRESCALE_VALUE_64
#define DRV_TMR_OPERATION_MODE_IDX0         DRV_TMR_OPERATION_MODE_16_BIT
#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX0     false
#define DRV_TMR_POWER_STATE_IDX0            

 // *****************************************************************************
/* USART Driver Configuration Options
*/
#define DRV_USART_INTERRUPT_MODE                    true

#define DRV_USART_BYTE_MODEL_SUPPORT                true

#define DRV_USART_READ_WRITE_MODEL_SUPPORT          false

#define DRV_USART_BUFFER_QUEUE_SUPPORT              false

#define DRV_USART_CLIENTS_NUMBER                    1
#define DRV_USART_INSTANCES_NUMBER                  2

#define DRV_USART_PERIPHERAL_ID_IDX0                USART_ID_2
#define DRV_USART_OPER_MODE_IDX0                    DRV_USART_OPERATION_MODE_NORMAL
#define DRV_USART_OPER_MODE_DATA_IDX0               
#define DRV_USART_INIT_FLAG_WAKE_ON_START_IDX0      false
#define DRV_USART_INIT_FLAG_AUTO_BAUD_IDX0          false
#define DRV_USART_INIT_FLAG_STOP_IN_IDLE_IDX0       false
#define DRV_USART_INIT_FLAGS_IDX0                   0
#define DRV_USART_BRG_CLOCK_IDX0                    33333333
#define DRV_USART_BAUD_RATE_IDX0                    115200
#define DRV_USART_LINE_CNTRL_IDX0                   DRV_USART_LINE_CONTROL_8NONE1
#define DRV_USART_HANDSHAKE_MODE_IDX0               DRV_USART_HANDSHAKE_NONE
#define DRV_USART_LINES_ENABLE_IDX0                 USART_ENABLE_TX_RX_USED
#define DRV_USART_XMIT_INT_SRC_IDX0                 INT_SOURCE_USART_2_TRANSMIT
#define DRV_USART_RCV_INT_SRC_IDX0                  INT_SOURCE_USART_2_RECEIVE
#define DRV_USART_ERR_INT_SRC_IDX0                  INT_SOURCE_USART_2_ERROR
#define DRV_USART_XMIT_INT_VECTOR_IDX0              INT_VECTOR_UART2_TX
#define DRV_USART_XMIT_INT_PRIORITY_IDX0            INT_PRIORITY_LEVEL5
#define DRV_USART_XMIT_INT_SUB_PRIORITY_IDX0        INT_SUBPRIORITY_LEVEL0
#define DRV_USART_RCV_INT_VECTOR_IDX0               INT_VECTOR_UART2_RX
#define DRV_USART_RCV_INT_PRIORITY_IDX0             INT_PRIORITY_LEVEL5
#define DRV_USART_RCV_INT_SUB_PRIORITY_IDX0         INT_SUBPRIORITY_LEVEL0
#define DRV_USART_ERR_INT_VECTOR_IDX0               INT_VECTOR_UART2_FAULT
#define DRV_USART_ERR_INT_PRIORITY_IDX0             INT_PRIORITY_LEVEL5
#define DRV_USART_ERR_INT_SUB_PRIORITY_IDX0         INT_SUBPRIORITY_LEVEL0


#define DRV_USART_POWER_STATE_IDX0                  SYS_MODULE_POWER_RUN_FULL

#define DRV_USART_PERIPHERAL_ID_IDX1                USART_ID_4
#define DRV_USART_OPER_MODE_IDX1                    DRV_USART_OPERATION_MODE_NORMAL
#define DRV_USART_OPER_MODE_DATA_IDX1               
#define DRV_USART_INIT_FLAG_WAKE_ON_START_IDX1      false
#define DRV_USART_INIT_FLAG_AUTO_BAUD_IDX1          false
#define DRV_USART_INIT_FLAG_STOP_IN_IDLE_IDX1       false
#define DRV_USART_INIT_FLAGS_IDX1                   0
#define DRV_USART_BRG_CLOCK_IDX1                    33333333
#define DRV_USART_BAUD_RATE_IDX1                    115200
#define DRV_USART_LINE_CNTRL_IDX1                   DRV_USART_LINE_CONTROL_8NONE1
#define DRV_USART_HANDSHAKE_MODE_IDX1               DRV_USART_HANDSHAKE_NONE
#define DRV_USART_LINES_ENABLE_IDX1                 USART_ENABLE_TX_RX_USED
#define DRV_USART_XMIT_INT_SRC_IDX1                 INT_SOURCE_USART_4_TRANSMIT
#define DRV_USART_RCV_INT_SRC_IDX1                  INT_SOURCE_USART_4_RECEIVE
#define DRV_USART_ERR_INT_SRC_IDX1                  INT_SOURCE_USART_4_ERROR
#define DRV_USART_XMIT_INT_VECTOR_IDX1              INT_VECTOR_UART4_TX
#define DRV_USART_XMIT_INT_PRIORITY_IDX1            INT_PRIORITY_LEVEL5
#define DRV_USART_XMIT_INT_SUB_PRIORITY_IDX1        INT_SUBPRIORITY_LEVEL0
#define DRV_USART_RCV_INT_VECTOR_IDX1               INT_VECTOR_UART4_RX
#define DRV_USART_RCV_INT_PRIORITY_IDX1             INT_PRIORITY_LEVEL5
#define DRV_USART_RCV_INT_SUB_PRIORITY_IDX1         INT_SUBPRIORITY_LEVEL0
#define DRV_USART_ERR_INT_VECTOR_IDX1               INT_VECTOR_UART4_FAULT
#define DRV_USART_ERR_INT_PRIORITY_IDX1             INT_PRIORITY_LEVEL5
#define DRV_USART_ERR_INT_SUB_PRIORITY_IDX1         INT_SUBPRIORITY_LEVEL0


#define DRV_USART_POWER_STATE_IDX1                  SYS_MODULE_POWER_RUN_FULL


// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: Application Configuration
// *****************************************************************************
// *****************************************************************************
/*** Application Defined Pins ***/

/*** Functions for LimitSwitchOutSig pin ***/
#define LimitSwitchOutSigToggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define LimitSwitchOutSigOn() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define LimitSwitchOutSigOff() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define LimitSwitchOutSigStateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define LimitSwitchOutSigStateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5, Value)

/*** Functions for Output_Gyro_IO3 pin ***/
#define Output_Gyro_IO3Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)
#define Output_Gyro_IO3On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)
#define Output_Gyro_IO3Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)
#define Output_Gyro_IO3StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)
#define Output_Gyro_IO3StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10, Value)

/*** Functions for Output_Gyro_IO4 pin ***/
#define Output_Gyro_IO4Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12)
#define Output_Gyro_IO4On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12)
#define Output_Gyro_IO4Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12)
#define Output_Gyro_IO4StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12)
#define Output_Gyro_IO4StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12, Value)

/*** Functions for Output_Master_IO6 pin ***/
#define Output_Master_IO6Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define Output_Master_IO6On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define Output_Master_IO6Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define Output_Master_IO6StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define Output_Master_IO6StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2, Value)

/*** Functions for Output_Master_IO5 pin ***/
#define Output_Master_IO5Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define Output_Master_IO5On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define Output_Master_IO5Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define Output_Master_IO5StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define Output_Master_IO5StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3, Value)

/*** Functions for LimitSwtichSignal pin ***/
#define LimitSwtichSignalStateGet() PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_9)


/*** Application Instance 0 Configuration ***/

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // _SYSTEM_CONFIG_H
/*******************************************************************************
 End of File
*/
