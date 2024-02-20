/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_usbx_device.c
 * @author  MCD Application Team
 * @brief   USBX Device applicative file
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
#include "app_usbx_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "usb_drd_fs.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define USBX_DEVICE_MEMORY_STACK_SIZE       (6 * 1024)

#define UX_DEVICE_APP_THREAD_STACK_SIZE   1024
#define UX_DEVICE_APP_THREAD_PRIO         10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

static UX_SLAVE_CLASS_CDC_ACM_PARAMETER cdc_acm_parameter;
static TX_THREAD ux_device_app_thread;

static TX_THREAD ux_cdc_read_thread;
static TX_THREAD ux_cdc_write_thread;
extern PCD_HandleTypeDef hpcd_USB_DRD_FS;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

static VOID app_ux_device_thread_entry(ULONG thread_input);

/* USER CODE END PFP */
/**
  * @brief  Application USBX Device Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_USBX_Device_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_USBX_Device_MEM_POOL */
	(void) byte_pool;
  /* USER CODE END MX_USBX_Device_MEM_POOL */

  /* USER CODE BEGIN MX_USBX_Device_Init */

	UCHAR *device_framework_full_speed;
	ULONG device_framework_fs_length;
	ULONG string_framework_length;
	ULONG language_id_framework_length;
	UCHAR *string_framework;
	UCHAR *language_id_framework;
	UCHAR *pointer;

	if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
	USBX_DEVICE_MEMORY_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
		return TX_POOL_ERROR;
	}

	/* Initialize USBX Memory */
	if (ux_system_initialize(pointer, USBX_DEVICE_MEMORY_STACK_SIZE, UX_NULL,
			0) != UX_SUCCESS) {
		return UX_ERROR;
	}

	/* Get Device Framework Full Speed and get the length */
	device_framework_full_speed = USBD_Get_Device_Framework_Speed(
	USBD_FULL_SPEED, &device_framework_fs_length);

	/* Get String Framework and get the length */
	string_framework = USBD_Get_String_Framework(&string_framework_length);

	/* Get Language Id Framework and get the length */
	language_id_framework = USBD_Get_Language_Id_Framework(
			&language_id_framework_length);

	/* Install the device portion of USBX */
	if (ux_device_stack_initialize(NULL,
			0U, device_framework_full_speed,
			device_framework_fs_length, string_framework,
			string_framework_length, language_id_framework,
			language_id_framework_length,
			UX_NULL) != UX_SUCCESS) {
		return UX_ERROR;
	}

	/* Initialize the cdc acm class parameters for the device */
	cdc_acm_parameter.ux_slave_class_cdc_acm_instance_activate =
			USBD_CDC_ACM_Activate;
	cdc_acm_parameter.ux_slave_class_cdc_acm_instance_deactivate =
			USBD_CDC_ACM_Deactivate;
	cdc_acm_parameter.ux_slave_class_cdc_acm_parameter_change =
			USBD_CDC_ACM_ParameterChange;

	/* Registers a slave class to the slave stack. The class is connected with
	 interface 0 */
	if (ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name,
	ux_device_class_cdc_acm_entry, 1, 0,
			(VOID*) &cdc_acm_parameter) != UX_SUCCESS) {
		return UX_ERROR;
	}

	/* Allocate the stack for device application main thread */
	if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
	UX_DEVICE_APP_THREAD_STACK_SIZE,
	TX_NO_WAIT) != TX_SUCCESS) {
		return TX_POOL_ERROR;
	}

	/* Create the device application main thread */
	if (tx_thread_create(&ux_device_app_thread, UX_DEVICE_APP_THREAD_NAME, app_ux_device_thread_entry,
			0, pointer, UX_DEVICE_APP_THREAD_STACK_SIZE, UX_DEVICE_APP_THREAD_PRIO,
			UX_DEVICE_APP_THREAD_PREEMPTION_THRESHOLD, UX_DEVICE_APP_THREAD_TIME_SLICE,
			UX_DEVICE_APP_THREAD_START_OPTION) != TX_SUCCESS) {
		return TX_THREAD_ERROR;
	}

	/* Allocate memory for the UX RX thread */
	tx_byte_allocate(byte_pool, (VOID**) &pointer, 1024, TX_NO_WAIT);
	/* Create the UX RX thread */
	tx_thread_create(&ux_cdc_read_thread, "cdc_acm_read_usbx_app_thread_entry",
			usbx_cdc_acm_read_thread_entry, 1, pointer, 1024, 20, 20,
			TX_NO_TIME_SLICE, TX_AUTO_START);
	/* Allocate memory for the UX TX thread */
	tx_byte_allocate(byte_pool, (VOID**) &pointer, 1024, TX_NO_WAIT);
	/* Create the UX TX thread */
	tx_thread_create(&ux_cdc_write_thread,
			"cdc_acm_write_usbx_app_thread_entry",
			usbx_cdc_acm_write_thread_entry, 1, pointer, 1025, 20, 20,
			TX_NO_TIME_SLICE, TX_AUTO_START);

  /* USER CODE END MX_USBX_Device_Init */

  return ret;
}

/* USER CODE BEGIN 1 */

/**
 * @brief  Function implementing app_ux_device_thread_entry.
 * @param  thread_input: User thread input parameter.
 * @retval none
 */
static VOID app_ux_device_thread_entry(ULONG thread_input) {

	MX_USB_DRD_FS_PCD_Init();
	HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x00, PCD_SNG_BUF, 0x14);
	HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x80, PCD_SNG_BUF, 0x54);
	HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x81, PCD_SNG_BUF, 0x94);
	HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x01, PCD_SNG_BUF, 0xD4);
	HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x82, PCD_SNG_BUF, 0x114);
	ux_dcd_stm32_initialize((ULONG) USB_DRD_FS, (ULONG) &hpcd_USB_DRD_FS);
	HAL_PCD_Start(&hpcd_USB_DRD_FS);

}

/* USER CODE END 1 */
