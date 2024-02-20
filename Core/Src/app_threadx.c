/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_threadx.c
 * @author  MCD Application Team
 * @brief   ThreadX applicative file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "neopixel.h"
#include "main_thread.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

TX_THREAD tx_neopixel_app_thread;
TX_THREAD tx_main_app_thread;

TX_EVENT_FLAGS_GROUP event_flags_neopixel;
TX_EVENT_FLAGS_GROUP event_flags_regulator;

TX_QUEUE queue_voltage;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
	(void) byte_pool;
  /* USER CODE END App_ThreadX_MEM_POOL */

  /* USER CODE BEGIN App_ThreadX_Init */

	CHAR *pointer;

	/* Allocate the stack for tx main app thread  */
	if (tx_byte_allocate(byte_pool, (VOID**) &pointer, TX_MAIN_APP_STACK_SIZE,
	TX_NO_WAIT) != TX_SUCCESS) {
		return TX_POOL_ERROR;
	}
	/* Create tx neopixel app thread.  */
	if (tx_thread_create(&tx_main_app_thread, "main app thread", tx_main_app_thread_entry, 0, pointer,
			TX_MAIN_APP_STACK_SIZE, TX_MAIN_APP_THREAD_PRIO, TX_MAIN_APP_THREAD_PREEMPTION_THRESHOLD,
			TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS) {
		return TX_THREAD_ERROR;
	}

	/* Allocate the stack for tx neopixel app thread  */
	if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
			TX_NEOPIXEL_APP_STACK_SIZE,
			TX_NO_WAIT) != TX_SUCCESS) {
		return TX_POOL_ERROR;
	}
	/* Create tx neopixel app thread.  */
	if (tx_thread_create(&tx_neopixel_app_thread, "neopixel app thread", tx_neopixel_app_thread_entry, 0, pointer,
			TX_NEOPIXEL_APP_STACK_SIZE, TX_NEOPIXEL_APP_THREAD_PRIO, TX_NEOPIXEL_APP_THREAD_PREEMPTION_THRESHOLD,
			TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS) {
		return TX_THREAD_ERROR;
	}

	/* Create the event flags group used by the neopixel driver. */
	tx_event_flags_create(&event_flags_neopixel, "regulator event flags");

	/* Create the event flags group used by the regulator driver. */
	tx_event_flags_create(&event_flags_regulator, "regulator event flags");

	/* Allocate the message queue. */
	tx_byte_allocate(byte_pool, (VOID**) &pointer, VOLTAGE_QUEUE_SIZE * sizeof(ULONG),
			TX_NO_WAIT);

	/* Create the message queue shared by threads 1 and 2. */
	tx_queue_create(&queue_voltage, "queue voltage", TX_1_ULONG, pointer,
			VOLTAGE_QUEUE_SIZE * sizeof(ULONG));

  /* USER CODE END App_ThreadX_Init */

  return ret;
}

/**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
