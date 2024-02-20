/*
 * main_thread.c
 *
 *  Created on: Jan 21, 2024
 *      Author: Petr Opravil
 */

#include "main_thread.h"
#include "neopixel.h"
#include "regulator.h"
#include "math.h"

extern TX_EVENT_FLAGS_GROUP event_flags_neopixel;
extern TX_EVENT_FLAGS_GROUP event_flags_regulator;

extern TX_QUEUE queue_voltage;

/**
 * @brief  Function implementing the tx_main_app_thread_entry thread.
 * @param  thread_input: Hardcoded to 0.
 * @retval None
 */
void tx_main_app_thread_entry(ULONG thread_input) {

	UINT status;
	ULONG actual_regulator_flags;

	ULONG voltage_output_raw;
	uint32_t voltage_output_mVolt;

	while (1) {

		/* Wait for neopixel event flag. */
		status = tx_event_flags_get(&event_flags_regulator, FLAG_REGULATOR_ALL,
		TX_OR_CLEAR, &actual_regulator_flags, TX_WAIT_FOREVER);

		/* Check status. */
		if (status == TX_SUCCESS) {
			switch (actual_regulator_flags) {

			case FLAG_REGULATOR_ON:

				/* Set neopixel event flag to turn ON LED. */
				status = tx_event_flags_set(&event_flags_neopixel,
				FLAG_NEOPIXEL_RED,
				TX_OR);
				/* Check status. */
				if (status != TX_SUCCESS)
					break;

				/* Enable regulator output. */
				regulator_enable();

				break;

			case FLAG_REGULATOR_OFF:

				/* Set neopixel event flag to turn ON LED. */
				status = tx_event_flags_set(&event_flags_neopixel,
				FLAG_NEOPIXEL_GREEN,
				TX_OR);
				/* Check status. */
				if (status != TX_SUCCESS)
					break;

				/* Disable regulator output. */
				regulator_disable();

				break;

			case FLAG_REGULATOR_SET:

				/* Retrieve a message from the queue. */
				status = tx_queue_receive(&queue_voltage, &voltage_output_raw,
						TX_NO_WAIT);

				/* Check completion status and make sure the message is what we
				 expected. */
				if (status != TX_SUCCESS)
					break;

				/* Convert voltage from raw data to a mVolt value. */
				voltage_output_mVolt = ((voltage_output_raw >> 4) * 1000) + ((voltage_output_raw & 0x0F) * 100);

				/* Set the output voltage. */
				regulator_set_voltage(voltage_output_mVolt);

				break;
			}
		}
	}
}
