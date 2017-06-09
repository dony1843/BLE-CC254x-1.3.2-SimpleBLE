/**************************************************************************************************
  Filename:       SimpleBLETest_Main.c
  Revised:        $Date: 2010-07-06 15:39:18 -0700 (Tue, 06 Jul 2010) $
  Revision:       $Revision: 22902 $

  Description:    This file contains the main and callback functions for
                  the Simple BLE Peripheral sample application.

  Copyright 2010 - 2011 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED 揂S IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

/**************************************************************************************************
 *                                           Includes
 **************************************************************************************************/
/* Hal Drivers */
#include "hal_types.h"
#include "hal_key.h"
#include "hal_timer.h"
#include "hal_drivers.h"
#include "hal_led.h"

/* OSAL */
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "OSAL_PwrMgr.h"
#include "osal_snv.h"
#include "OnBoard.h"

/**************************************************************************************************
 * FUNCTIONS
 **************************************************************************************************/

/* This callback is triggered when a key is pressed */
void MSA_Main_KeyCallback(uint8 keys, uint8 state);

/**************************************************************************************************
 * @fn          main
 *
 * @brief       Start of application.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
// 任何8051单片机c程序， 都是由 main 函数开始的，
// 我们拿到一份代码，首先需要找到main函数
int main(void)
{
  /* Initialize hardware */
  HAL_BOARD_INIT();      //初始化时钟稳定时钟等等

  // Initialize board I/O
  //冷启动，关闭了led灯与中断， 一边接下来的各种初始化不受干扰
  InitBoard( OB_COLD );

  /* Initialze the HAL driver */
  HalDriverInit();   //各种驱动的初始化、如按键、lcd、adc、usb、uart等

  /* Initialize NV system */
  //snv 内部用于保存配对数据或你的用户自定义数据的一段flash，4kB空间
  osal_snv_init(); 

  /* Initialize LL */

  /* Initialize the operating system */
  //oasl 操作系统初始化, 包含内存分配、消息队列、定时器、电源管理和任务等
  osal_init_system(); 

  /* Enable interrupts */
  HAL_ENABLE_INTERRUPTS();// 开启全局中断

  // Final board initialization
  InitBoard( OB_READY );      //设置标志标示系统初始化完毕 

  #if defined ( POWER_SAVING )
  // 如果你使能了低功耗， 就启动低功耗模式，
    osal_pwrmgr_device( PWRMGR_BATTERY );
  #endif
/*
低功耗部分
1.如何总是在PM1
  osal_pwrmgr_device( PWRMGR_ALWAYS_ON );
2.如何进入PM2
  osal_pwrmgr_device( PWRMGR_BATTERY );在空闲的时候就会进入到PM2模式
3.如何进入PM3
  存在连接就断开连接，存在广播就停掉广播，并确认自己创建的所有定时任务都已关闭，
  则系统应该就会进入PM3模式，只能进行外部中断唤醒
*/

  /* Start OSAL */
  osal_start_system(); // No Return from here
/* osal 操作系统启动，实际上是一个大循环，只是检查相对应的标志位，
就指定相对应的任务,看到这里，同学们应该往哪里看呢?其实，这已经是尽头了?那么我们的应用程序是在哪里写的呢
其实是在上面的 上面的函数 osal_init_system 里就初始化了，现在回过头去看看
osal_init_system 这个函数内部就知道了
*/    
  return 0;
}

/**************************************************************************************************
                                           CALL-BACKS
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/
