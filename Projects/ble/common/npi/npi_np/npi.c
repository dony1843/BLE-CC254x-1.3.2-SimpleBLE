/*******************************************************************************
  Filename:       npi.c
  Revised:        $Date: 2008-06-11 14:30:47 -0700 (Wed, 11 Jun 2008) $
  Revision:       $Revision: 17210 $

  Description:    This file contains the Network Processor Interface (NPI),
                  which abstracts the physical link between the Application
                  Processor (AP) and the Network Processor (NP). The NPI
                  serves as the HAL's client for the SPI and UART drivers, and
                  provides API and callback services for its client.

  Copyright 2006-2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include "hal_board.h"
#include "npi.h"
#include "OSAL.h"
/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * PROTOTYPES
 */

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn          NPI_InitTransport
 *
 * @brief       This routine initializes the transport layer and opens the port
 *              of the device. Note that based on project defines, either the
 *              UART, USB (CDC), or SPI driver can be used.
 *
 * input parameters
 *
 * @param       npiCback - User callback function when data is available.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void NPI_InitTransport( npiCBack_t npiCBack )
{
  halUARTCfg_t uartConfig;

  // configure UART
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = NPI_UART_BR;
  uartConfig.flowControl          = NPI_UART_FC;
  uartConfig.flowControlThreshold = NPI_UART_FC_THRESHOLD;
  uartConfig.rx.maxBufSize        = NPI_UART_RX_BUF_SIZE;
  uartConfig.tx.maxBufSize        = NPI_UART_TX_BUF_SIZE;
  uartConfig.idleTimeout          = NPI_UART_IDLE_TIMEOUT;
  uartConfig.intEnable            = NPI_UART_INT_ENABLE;
  uartConfig.callBackFunc         = (halUARTCBack_t)npiCBack;

  // start UART
  // Note: Assumes no issue opening UART port.
  (void)HalUARTOpen( NPI_UART_PORT, &uartConfig );

  return;
}

void NPI_InitTransportEx( npiCBack_t npiCBack, uint8 baudrate, uint8 parity, uint8 stopbit)
{
  halUARTCfg_t uartConfig;

  // configure UART
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = baudrate;//NPI_UART_BR;
  //uartConfig.parity              = parity;
  //uartConfig.stopbit              = stopbit;
  uartConfig.flowControl          = NPI_UART_FC;
  uartConfig.flowControlThreshold = NPI_UART_FC_THRESHOLD;
  uartConfig.rx.maxBufSize        = NPI_UART_RX_BUF_SIZE;
  uartConfig.tx.maxBufSize        = NPI_UART_TX_BUF_SIZE;
  uartConfig.idleTimeout          = NPI_UART_IDLE_TIMEOUT;
  uartConfig.intEnable            = NPI_UART_INT_ENABLE;
  uartConfig.callBackFunc         = (halUARTCBack_t)npiCBack;

  // start UART
  // Note: Assumes no issue opening UART port.
  (void)HalUARTOpen( NPI_UART_PORT, &uartConfig );

  return;
}

/*******************************************************************************
 * @fn          NPI_InitTransport
 *
 * @brief       This routine initializes the transport layer and opens the port
 *              of the device. Note that based on project defines, either the
 *              UART, USB (CDC), or SPI driver can be used.
 *
 * input parameters
 *
 * @param       npiCback - User callback function when data is available.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void NPI_InitTransport_Usb( npiCBack_t npiCBack )
{
  halUARTCfg_t uartConfig;

  // configure UART
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = NPI_UART_BR;
  uartConfig.flowControl          = NPI_UART_FC;
  uartConfig.flowControlThreshold = NPI_UART_FC_THRESHOLD;
  uartConfig.rx.maxBufSize        = NPI_UART_RX_BUF_SIZE;
  uartConfig.tx.maxBufSize        = NPI_UART_TX_BUF_SIZE;
  uartConfig.idleTimeout          = NPI_UART_IDLE_TIMEOUT;
  uartConfig.intEnable            = NPI_UART_INT_ENABLE;
  uartConfig.callBackFunc         = (halUARTCBack_t)npiCBack;

  // start UART
  // Note: Assumes no issue opening UART port.
  (void)HalUARTOpen( NPI_UART_PORT, &uartConfig );

  return;
}


/*******************************************************************************
 * @fn          NPI_ReadTransport
 *
 * @brief       This routine reads data from the transport layer based on len,
 *              and places it into the buffer.
 *
 * input parameters
 *
 * @param       buf - Pointer to buffer to place read data.
 * @param       len - Number of bytes to read.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Returns the number of bytes read from transport.
 */
uint16 NPI_ReadTransport( uint8 *buf, uint16 len )
{
  return( HalUARTRead( NPI_UART_PORT, buf, len ) );
}


/*******************************************************************************
 * @fn          NPI_WriteTransport
 *
 * @brief       This routine writes data from the buffer to the transport layer.
 *
 * input parameters
 *
 * @param       buf - Pointer to buffer to write data from.
 * @param       len - Number of bytes to write.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Returns the number of bytes written to transport.
 */
uint16 NPI_WriteTransport( uint8 *buf, uint16 len )
{
#if 0
// amomcu �Ż��� �����������ϴ�ʱ��Ҫ�ȴ����ݷ�������ٷ��ͣ� �������ݾͻᶪʧ
// ��Ȼ�� Ҳ���������ͻ������� �����ڴ����ޣ� ֻ����ô����     
  uint16 ret = 0;
  // �ú�����ʱʱ��Ϊ1ms�� ��ʾ������������ ������ ������С  --amomcu.com
  extern void simpleBLE_Delay_1ms(int times);

  // ������Խ�ͣ� ��ʱҪ��Խ�� �봫�����ݿ죬����Ҫ115200�����ʵ� --amomcu.com
  // ���ﲻ����ʱ̫��ʱ�䣬 �����Ͽ��������ӵ�   --amomcu.com
  for(uint16 i = 0; i < 100; i++)
  {
     ret =  HalUARTWrite( NPI_UART_PORT, buf, len );   
     if(ret == len)
     {
        break;
     }
     simpleBLE_Delay_1ms(10);
  }

  return ret;  
#else
  return( HalUARTWrite( NPI_UART_PORT, buf, len ) );
#endif
}


/*******************************************************************************
 * @fn          NPI_RxBufLen
 *
 * @brief       This routine returns the number of bytes in the receive buffer.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Returns the number of bytes in the receive buffer.
 */
uint16 NPI_RxBufLen( void )
{
  return( Hal_UART_RxBufLen( NPI_UART_PORT ) );
}


/*******************************************************************************
 * @fn          NPI_GetMaxRxBufSize
 *
 * @brief       This routine returns the max size receive buffer.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Returns the max size of the receive buffer.
 */
uint16 NPI_GetMaxRxBufSize( void )
{
  return( NPI_UART_RX_BUF_SIZE );
}


/*******************************************************************************
 * @fn          NPI_GetMaxTxBufSize
 *
 * @brief       This routine returns the max size transmit buffer.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Returns the max size of the transmit buffer.
 */
uint16 NPI_GetMaxTxBufSize( void )
{
  return( NPI_UART_TX_BUF_SIZE );
}

/*
��ӡһ���ַ���
str�����԰���0x00�����ǽ�β
*/
void NPI_PrintString(uint8 *str)
{
    NPI_WriteTransport(str, osal_strlen((char*)str));
}
/*
��ӡָ���ĸ�ʽ����ֵ
����
title,ǰ׺�ַ���
value,��Ҫ��ʾ����ֵ
format,��Ҫ��ʾ�Ľ��ƣ�ʮ����Ϊ10,ʮ������Ϊ16
*/
void NPI_PrintValue(char *title, uint16 value, uint8 format)
{
  uint8 tmpLen;
  uint8 buf[128];
  uint32 err;

  tmpLen = (uint8)osal_strlen( (char*)title );
  osal_memcpy( buf, title, tmpLen );
  buf[tmpLen] = ' ';
  err = (uint32)(value);
  _ltoa( err, &buf[tmpLen+1], format );
  NPI_PrintString(buf);		
}

// �ú�����ʱʱ��Ϊ1ms�� ��ʾ������������ ������ ������С  --amomcu.com
void simpleBLE_Delay_1ms(int times)
{
  while(times--)
  {
      int i=0;
      for(i=1500;i>0;i--)
      {
    	  asm("nop");
      }
  }
}


/*******************************************************************************
 ******************************************************************************/
