/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    ux_device_cdc_acm.c
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
#include "ux_device_cdc_acm.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "main.h"
#include "regulator.h"

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

UX_SLAVE_CLASS_CDC_ACM *cdc_acm;

extern TX_EVENT_FLAGS_GROUP event_flags_regulator;
extern TX_QUEUE queue_voltage;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * @brief  USBD_CDC_ACM_Activate
 *         This function is called when insertion of a CDC ACM device.
 * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
 * @retval none
 */
VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance) {
	/* USER CODE BEGIN USBD_CDC_ACM_Activate */
	cdc_acm = (UX_SLAVE_CLASS_CDC_ACM*) cdc_acm_instance;
	/* USER CODE END USBD_CDC_ACM_Activate */

	return;
}

/**
 * @brief  USBD_CDC_ACM_Deactivate
 *         This function is called when extraction of a CDC ACM device.
 * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
 * @retval none
 */
VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance) {
	/* USER CODE BEGIN USBD_CDC_ACM_Deactivate */
	UX_PARAMETER_NOT_USED(cdc_acm_instance);
	/* USER CODE END USBD_CDC_ACM_Deactivate */

	return;
}

/**
 * @brief  USBD_CDC_ACM_ParameterChange
 *         This function is invoked to manage the CDC ACM class requests.
 * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
 * @retval none
 */
VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance) {
	/* USER CODE BEGIN USBD_CDC_ACM_ParameterChange */
	UX_PARAMETER_NOT_USED(cdc_acm_instance);
	/* USER CODE END USBD_CDC_ACM_ParameterChange */

	return;
}

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

VOID usbx_cdc_acm_write_thread_entry(ULONG thread_input) {
	/* Private Variables */
	ULONG tx_actual_length;
	const uint8_t message[] = "USBX Application Running!\r\n";
	while (1) {
		ux_device_class_cdc_acm_write(cdc_acm, (UCHAR*) (message),
				sizeof(message), &tx_actual_length);
		tx_thread_sleep(1000);
	}
}

VOID usbx_cdc_acm_read_thread_entry(ULONG thread_input) {
	/* Private Variables */

	UINT status;
	ULONG rx_actual_length;

	uint8_t UserRxBuffer[64];
	ULONG voltage_raw;

	/* Infinite Loop */
	while (1) {
		if (cdc_acm != UX_NULL) {

			ux_device_class_cdc_acm_read(cdc_acm, (UCHAR*) UserRxBuffer, 64,
					&rx_actual_length);

			if (rx_actual_length > 0) {

				switch (UserRxBuffer[0]) {

				case 0x01:
					/* Set regulator event flag to turn OFF the regulator. */
					status = tx_event_flags_set(&event_flags_regulator,
					FLAG_REGULATOR_OFF,
					TX_OR);

					/* Check status. */
					if (status != TX_SUCCESS)
						break;
					break;

				case 0x02:
					/* Set regulator event flag to turn ON the regulator. */
					status = tx_event_flags_set(&event_flags_regulator,
					FLAG_REGULATOR_ON,
					TX_OR);

					/* Check status. */
					if (status != TX_SUCCESS)
						break;
					break;

				case 0x03:
					if (rx_actual_length >= 2) {

						/* Voltage value will be set by the second character */
						voltage_raw = UserRxBuffer[1];

						/* Send message to queue voltage. */
						status = tx_queue_send(&queue_voltage,
								&voltage_raw,
								TX_WAIT_FOREVER);

						/* Check completion status. */
						if (status != TX_SUCCESS)
							break;

						/* Set regulator event flag to set the regulator. */
						status = tx_event_flags_set(&event_flags_regulator,
						FLAG_REGULATOR_SET,
						TX_OR);

						/* Check status. */
						if (status != TX_SUCCESS)
							break;
						break;

					}
				}
			}
		}
	}
}

/* USER CODE END 1 */
