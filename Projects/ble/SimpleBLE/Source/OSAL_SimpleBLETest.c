/**************************************************************************************************
  Filename:       OSAL_SimpleBLETest.c
  Revised:        $Date: 2010-08-01 14:02:20 -0700 (Sun, 01 Aug 2010) $
  Revision:       $Revision: 23255 $

  Description:    This file contains function that allows user setup tasks


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
  PROVIDED ìAS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
 *                                            INCLUDES
 **************************************************************************************************/
#include "hal_types.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"

/* HAL */
#include "hal_drivers.h"

/* LL */
#include "ll.h"

/* HCI */
#include "hci_tl.h"

#if defined ( OSAL_CBTIMER_NUM_TASKS )
  #include "osal_cbTimer.h"
#endif

/* L2CAP */
#include "l2cap.h"

/* gap */
#include "gap.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"

/* GATT */
#include "gatt.h"

#include "gattservapp.h"

/* Profiles */
#if defined ( PLUS_BROADCASTER )
  #include "peripheralBroadcaster.h"
#else
  #include "peripheral.h"
#endif

/* Application */
#include "simpleBLETest.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */

// The order in this table must be identical to the task initialization calls below in osalInitTask.
// ’‚∏ˆ±Ìœ‡µ±÷ÿ“™£¨ ∂º «∂®“Â¡À∏˜∏ˆ»ŒŒÒµƒ ¬º˛÷¥––∫Ø ˝£¨ø…“‘¿ÌΩ‚≥…ªÿµ˜∫Ø ˝£¨ªÚ ¬º˛÷¥––∫Ø ˝
const pTaskEventHandlerFn tasksArr[] =
{
  LL_ProcessEvent,                                                  // task 0
  Hal_ProcessEvent,                                                 // task 1
  HCI_ProcessEvent,                                                 // task 2
#if defined ( OSAL_CBTIMER_NUM_TASKS )
  OSAL_CBTIMER_PROCESS_EVENT( osal_CbTimerProcessEvent ),           // task 3
#endif
  L2CAP_ProcessEvent,                                               // task 4
  GAP_ProcessEvent,                                                 // task 5
  GATT_ProcessEvent,                                                // task 6
  SM_ProcessEvent,                                                  // task 7
  GAPRole_ProcessEvent,                                             // task 8
  GAPBondMgr_ProcessEvent,                                          // task 9
  GATTServApp_ProcessEvent,                                         // task 10
  SimpleBLETest_ProcessEvent                                  // task 11  
  // ’‚∏ˆ «Œ“√«µƒ”¶”√≥Ã–Úµƒ ¬º˛¥¶¿Ì∫Ø ˝
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
 /*
ø…“‘ø¥µΩ‘⁄ﬂ@ÇÄîµΩMµƒ∂®¡x÷–£¨√øÇÄ≥…ÜT∂º «»ŒÑ’µƒàÃ––∫Øîµ£¨∞¥’’»ŒÑ’µƒÉûœ»ºâ≈≈–Ú£¨
ÅK«“‘⁄osalInitTasks÷–≥ı ºªØµƒïr∫Ú£¨Œ“ÇÉø…“‘ø¥µΩ√øÇÄ»ŒÑ’∂º”–“ªÇÄå¶ë™µƒ≥ı ºªØ∫Øîµ£
¨ÅK«“Ç˜ﬂf¡À“ªÇÄtaskID£¨¥ÀIDèƒ0È_ º◊‘‘ˆ£¨ﬂ@—Y”–“ª¸c∑«≥£÷ÿ“™£¨
≥ı ºªØµƒÌò–Ú∫Õ»ŒÑ’îµΩMµƒ∂®¡xÌò–Ú «“ªò”µƒ£¨
ﬂ@æÕ±£◊C¡ÀŒ“ÇÉΩo»ŒÑ’∞l…˙œ˚œ¢ªÚ ¬º˛ïrƒ‹âÚú ¥_µƒÇ˜ﬂfµΩœ‡ë™µƒ»ŒÑ’Ãé¿Ì∫Øîµ°£
 */
void osalInitTasks( void )
{
  uint8 taskID = 0;
  // ∑÷≈‰»ŒŒÒ ¬º˛ø’º‰£¨’‚¿Ô≤…”√∂ØÃ¨µƒ∑Ω∑®¿¥◊ˆ£¨±»Ωœ∑Ω±„‘⁄tasksArr∂¯¥˙¬Î–ﬁ∏ƒ…Ÿ
  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

  /* LL Task */
  LL_Init( taskID++ );

  /* Hal Task */
  Hal_Init( taskID++ );

  /* HCI Task */
  HCI_Init( taskID++ );

#if defined ( OSAL_CBTIMER_NUM_TASKS )
  /* Callback Timer Tasks */
  osal_CbTimerInit( taskID );
  taskID += OSAL_CBTIMER_NUM_TASKS;
#endif

  /* L2CAP Task */
  L2CAP_Init( taskID++ );

  /* GAP Task */
  GAP_Init( taskID++ );

  /* GATT Task */
  GATT_Init( taskID++ );

  /* SM Task */
  SM_Init( taskID++ );

  /* Profiles */
  GAPRole_Init( taskID++ );  // Ω«…´≥ı ºªØ
  GAPBondMgr_Init( taskID++ );

  GATTServApp_Init( taskID++ );

  /* Application */
  SimpleBLETest_Init( taskID );     //’‚∏ˆæÕ «Œ“√«µƒ”¶”√≥Ã–Ú≥ı ºªØ
}

/*********************************************************************
*********************************************************************/
