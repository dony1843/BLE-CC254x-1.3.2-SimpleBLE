/**************************************************************************************************
  Filename:       SimpleBLETest.c
  Revised:        $Date: 2010-08-06 08:56:11 -0700 (Fri, 06 Aug 2010) $
  Revision:       $Revision: 23333 $

  Description:    This file contains the Simple BLE Peripheral sample application
                  for use with the CC2540 Bluetooth Low Energy Protocol Stack.

  Copyright 2010 - 2013 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "hal_board_cfg.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_lcd.h"
#include "hal_sensor.h"
#include "hal_i2c.h"
#include "simpleBLETest.h"
#include "npi.h"
#include "stdio.h"
#include "AFE4404_Drv.h"

/*********************************************************************
 * MACROS
 */
// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD                   2000 //毫秒

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 SimpleBLETest_TaskID;   // Task ID for internal task/event processing

uint16 afe4404_count = 0;

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
static void simpleBLECentral_HandleKeys( uint8 shift, uint8 keys );
static void simpleBLECentral_ProcessOSALMsg( osal_event_hdr_t *pMsg );

static void NpiSerialCallback( uint8 port, uint8 events );

/*********************************************************************
 * @fn      SimpleBLETest_Init
 *
 * @brief   Initialization function for the Simple BLE Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SimpleBLETest_Init( uint8 task_id )
{ //保存任务id到全局变量   
  SimpleBLETest_TaskID = task_id;

  // 串口初始化 波特率默认是115200, 形参是回调函数
  NPI_InitTransport(NpiSerialCallback);

  // 按长度输出
  NPI_WriteTransport("SimpleBLETest_Init\r\n", 20);

  // 按字符串输出
  NPI_PrintString("SimpleBLETest_Init2\r\n");

  // 可以输出一个值，用10进制表示
  NPI_PrintValue("AmoMcu 1 =", 168, 10);
  NPI_PrintString("\r\n");
  
  // 可以输出一个值，用16进制表示
  NPI_PrintValue("AmoMcu 2 = 0x", 0x88, 16);
  NPI_PrintString("\r\n");
  
  //初始化ADC
 // HalAdcInit();
  
  // Register for all key events - This app will handle all key events
  RegisterForKeys( SimpleBLETest_TaskID );

  // Setup a delayed profile startup
  /*
  设置一个任务， 这么做的目的是按照多任务处理的方法来做
  SimpleBLETest_ProcessEvent 就是处理 SBP_START_DEVICE_EVT
  */
  osal_set_event( SimpleBLETest_TaskID, SBP_START_DEVICE_EVT );
}

/*********************************************************************
 * @fn      SimpleBLETest_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
// 这个是我们的应用程序的事件处理函数
uint16 SimpleBLETest_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // OSAL required parameter that isn't used in this function

  // SYS_EVENT_MSG 这是系统事件比如按键事件蓝牙读写事件处理，都会置这个事件
  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = osal_msg_receive( SimpleBLETest_TaskID )) != NULL )
    {
      simpleBLECentral_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      // Release the OSAL message
      VOID osal_msg_deallocate( pMsg );
    }
    
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // 这个是我们应用程序自定义的事件，SBP_START_DEVICE_EVT 的值被定义为 0x0001， 
  // 实际上我们可以定义 16个事件， 第一的时候是以位来定义的
  // 
  if ( events & SBP_START_DEVICE_EVT )
  {
 //   HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);      // 点亮led1
    // 第一次启动定时器
    if ( SBP_PERIODIC_EVT_PERIOD )
    { 
   // SBP_PERIODIC_EVT_PERIOD 为5000ms后产生SBP_PERIODIC_EVT 事件
  //  osal_start_reload_timer( SimpleBLETest_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
      osal_start_timerEx( SimpleBLETest_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );  //only once

      // 下面的这个函数是自动重装的函数，只要启动之后，就会定时 产生 SBP_PERIODIC_EVT 事件
      //osal_start_reload_timer( SimpleBLETest_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    }
   //AFE4404采集程序
   afe_ppg_start();

    // 返回这个， 告诉osal，这个实践你已经处理了
    return ( events ^ SBP_START_DEVICE_EVT );
  }
  
  // 产生定时器事件
  if ( events & SBP_PERIODIC_EVT )
  { 
    osal_stop_timerEx( SimpleBLETest_TaskID, SBP_PERIODIC_EVT );
 //   uint8 i = AFE4404_hw_iic_Test(); 
 //   NPI_PrintValue("iic status: 0x", i, 16);
 //   NPI_PrintString("\r\n");
    //AFE4404采集程序
  // afe_ppg_start();
    
    //osal_start_timerEx( SimpleBLETest_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    
    return (events ^ SBP_PERIODIC_EVT);
  }
  
  if( events & AFE4404_ADC_READY_EVT )
  {
    if( !(AFE_ADC_RDY_PORT & AFE_ADC_RDY_BIT) )
    {
  /*    
      afe4404_count++;
    
      NPI_PrintValue("", afe4404_count, 10);
      NPI_PrintString("\r\n");
      if(afe4404_count == 1000)
      {
        afe4404_count = 0;
      }
  */    
      afe4404_interrupt_process();
    }
    return (events ^ AFE4404_ADC_READY_EVT);
  }

  // Discard unknown events
  return 0;
}


/*********************************************************************
 * @fn      simpleBLECentral_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void simpleBLECentral_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
    case KEY_CHANGE:
      simpleBLECentral_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
      break;

    case GATT_MSG_EVENT:
      //simpleBLECentralProcessGATTMsg( (gattMsgEvent_t *) pMsg );
      break;
  }
}

/*********************************************************************
 * @fn      simpleBLECentral_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void simpleBLECentral_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter

//  NPI_PrintValue( "key = 0x", keys, 16 );

  // 
  if ( keys & HAL_KEY_SW_SOS )
  {
    NPI_PrintString( "HAL_KEY_SW_SOS\r\n" );
    uint16 adc;
      
   // Configure ADC and perform a read
    HalAdcSetReference( HAL_ADC_REF_AVDD );   //设置AVDD为参考电压
    adc = HalAdcRead( HAL_ADC_CHN_AIN0, HAL_ADC_RESOLUTION_12 );
  //  adc = HalAdcRead( HAL_ADC_CHN_VDD3, HAL_ADC_RESOLUTION_12 );

    NPI_PrintValue( "ADC", adc, 10 );
    NPI_PrintString("\r\n");
    
    char str[32];
    float volt = (float)( adc - 100 )*2.87 / 2048;
       
    // 格式化
    sprintf(str, "volt = %.2fv", volt);
       
    //显示字符串在某一行
    NPI_PrintString( str );
    NPI_PrintString("\r\n");
  }
  
  if ( keys & HAL_KEY_SW_MEASURE )
  {
    NPI_PrintString( "HAL_KEY_SW_MEASURE\r\n" );
  }
}


// 串口回调函数， 下面把该回调函数里实现的功能讲解一下
/*
1, 思路:  当串口收到数据后，就会马上调用以下回调函数，在实际测试中发现，此回调
函数调用频繁， 如果你不执行NPI_ReadTransport函数进行读取， 那么这个回调函数就会
频繁地被执行，但是，你通过串口发送一段数据， 你本意是想处理这一完整一段的数据，所以，
我们在下面引入了时间的处理方法， 也即接收的数据够多或者超时，就读取一次数据， 
然后根据当前的状态决定执行，如果没有连接上，就把所有数据当做AT命令处理， 如果连接
上了，就把数据送到对端。  ---------------amomcu   2014.08.17
*/
static void NpiSerialCallback( uint8 port, uint8 events )
{
    (void)port;//加个 (void)，是未了避免编译告警，明确告诉缓冲区不用理会这个变量

    if (events & (HAL_UART_RX_TIMEOUT | HAL_UART_RX_FULL))   //串口有数据
    {
        uint8 numBytes = 0;

        numBytes = NPI_RxBufLen();           //读出串口缓冲区有多少字节
        
        if(numBytes == 0)
        {
            return;
        }
        else
        {
            //申请缓冲区buffer
            uint8 *buffer = osal_mem_alloc(numBytes);
            if(buffer)
            {
                //读取读取串口缓冲区数据，释放串口数据   
                NPI_ReadTransport(buffer,numBytes);   

                //把收到的数据发送到串口-实现回环 
                NPI_WriteTransport(buffer, numBytes);  

                //释放申请的缓冲区
                osal_mem_free(buffer);
            }
        }
    }
}

/***************************************************************************************************
 *                                    INTERRUPT SERVICE ROUTINE
 ***************************************************************************************************/

/**************************************************************************************************
 * @fn      halKeyPort1Isr
 *
 * @brief   Port1 ISR
 *
 * @param
 *
 * @return
 **************************************************************************************************/
HAL_ISR_FUNCTION( halKeyPort1Isr, P1INT_VECTOR )
{
  HAL_ENTER_ISR();

  if ((HAL_KEY_SW_SOS_PXIFG & HAL_KEY_SW_SOS_BIT) || (HAL_KEY_SW_MEASURE_PXIFG & HAL_KEY_SW_MEASURE_BIT))
  {
    halProcessKeyInterrupt();
    
    /*
    Clear the CPU interrupt flag for Port_1
    PxIFG has to be cleared before PxIF
    */
    HAL_KEY_SW_SOS_PXIFG = 0;
    HAL_KEY_SW_MEASURE_PXIFG = 0;
    
    HAL_KEY_CPU_PORT_1_IF = 0;
  }
  
  if(AFE_ADC_RDY_PXIFG & AFE_ADC_RDY_BIT)
  {
    
    afe4404_interrupt_process();
  /*
    afe4404_count++;
    
    NPI_PrintValue("", afe4404_count, 10);
    NPI_PrintString("\r\n");
    if(afe4404_count == 1000)
    {
      afe4404_count = 0;
    }
  */
  //  osal_start_timerEx( SimpleBLETest_TaskID, AFE4404_ADC_READY_EVT, 1 );
    
    /*
    Clear the CPU interrupt flag for Port_1
    PxIFG has to be cleared before PxIF
  */
    AFE_ADC_RDY_PXIFG = 0;

    AFE_ADC_RDY_CPU_PORT_1_IF = 0;
  }

  
  
  

  CLEAR_SLEEP_MODE();

  HAL_EXIT_ISR();

  return;
}

/*********************************************************************
*********************************************************************/
