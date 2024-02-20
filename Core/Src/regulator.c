/*
 * regulator.c
 *
 *  Created on: Jan 21, 2024
 *      Author: Petr Opravil
 */

#include "regulator.h"
#include "main.h"
#include "stdlib.h"
#include "math.h"
#include "tx_api.h"
#include "spi.h"
#include "adc.h"

#define REGULATOR_WRITE_POTENTIOMETER		0x11
#define VDDA_APPLI							3300
#define ADC_CONVERSION_TIMEOUT				100

extern ADC_HandleTypeDef hadc1;

uint8_t adc_conversion_complete_flag;

/**
 * @brief  Function implementing the regulator_enable function.
 * @retval None
 */
void regulator_enable(void) {

	uint8_t spi_write_data_buf[2];

	/* Configure the first byte to enable writing to potentiometer. */
	spi_write_data_buf[1] = REGULATOR_WRITE_POTENTIOMETER;

	/* Set potentiometer value to output 1V8. */
	spi_write_data_buf[0] = 0x7A;

	/* SPI transmit data. */
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, RESET);
	HAL_SPI_Transmit(&hspi1, spi_write_data_buf, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, SET);

	/* Delay task for 10 ms. */
	tx_thread_sleep(1);

	HAL_GPIO_WritePin(REG_EN_GPIO_Port, REG_EN_Pin, SET);

}

/**
 * @brief  Function implementing the regulator_disable function.
 * @retval None
 */
void regulator_disable(void) {

	HAL_GPIO_WritePin(REG_EN_GPIO_Port, REG_EN_Pin, RESET);

}

/**
 * @brief  Function implementing the regulator_set_voltage function.
 * @param  output_voltage: Output voltage to be set in millivolts.
 * @retval None
 */
void regulator_set_voltage(uint32_t output_voltage) {

	uint8_t spi_write_data_buf[2];
	uint16_t adc_prev_value = 0;
	uint16_t adc_curr_value = 0;
	uint8_t potentiometer_value_est;

	if (output_voltage < 3600) {

		/* Run the ADC calibration */
		if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
			/* Calibration Error */
			Error_Handler();
		}

		/* Delay task for 10 ms. */
		tx_thread_sleep(1);

		/* Configure the first byte to enable writing to potentiometer. */
		spi_write_data_buf[1] = REGULATOR_WRITE_POTENTIOMETER;

		/* Calculate estimated potentiometer value for set voltage. */
		potentiometer_value_est = 273.89f
				* powf((output_voltage / 1000.0f), -1.366f);

		/* Sweep 5 values around estimated value or less. */
		for (int16_t i = 2; i > -3; i--) {

			/* Set potentiometer value with added sweep. */
			spi_write_data_buf[0] = potentiometer_value_est + i;

			/* SPI transmit data. */
			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, RESET);
			HAL_SPI_Transmit(&hspi1, spi_write_data_buf, 1, HAL_MAX_DELAY);
			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, SET);

			/* Delay task for 20 ms for output stability. */
			tx_thread_sleep(2);

			/* Save the last ADC value. */
			adc_prev_value = adc_curr_value;

			/* Clear current ADC variable. */
			adc_curr_value = 0;

			/* Measure the same value 10 times. */
			for (int16_t j = 0; j < 10; j++) {

				/* Start ADC conversion. */
				HAL_ADC_Start_IT(&hadc1);
				adc_conversion_complete_flag = 0;

				/* Wait for ADC conversion completion. */
				for (int16_t k = 0; k < ADC_CONVERSION_TIMEOUT; k++) {

					/* Exit delay when conversion finishes. */
					if (adc_conversion_complete_flag)
						break;
					HAL_Delay(1);
				}

				/* Add read ADC value to the sum. */
				adc_curr_value +=
						__LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, HAL_ADC_GetValue(&hadc1),
								LL_ADC_RESOLUTION_12B) * 2;

			}

			/* Calculate average of values. */
			adc_curr_value /= 10;

			/* Check if previous value is closer than the current one. */
			if (abs(output_voltage - adc_prev_value)
					< abs(output_voltage - adc_curr_value)) {

				/* Set the last closer value. */
				spi_write_data_buf[0] = potentiometer_value_est + 1 + i;

				/* SPI transmit data. */
				HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, RESET);
				HAL_SPI_Transmit(&hspi1, spi_write_data_buf, 1, HAL_MAX_DELAY);
				HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, SET);
				break;
			}

		}

		/* Stop ADC. */
		HAL_ADC_Stop(&hadc1);

	}
}

/**
 * @brief  Function implementing ADC conversion complete callback.
 * @param  hadc: ADC handle.
 * @retval None
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {

	adc_conversion_complete_flag = 1;

}

