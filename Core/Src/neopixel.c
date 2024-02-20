/*
 * neopixel.c
 *
 *  Created on: Jan 20, 2024
 *      Author: Petr Opravil
 */

#include "neopixel.h"

#include "tim.h"
#include "math.h"

extern TX_EVENT_FLAGS_GROUP event_flags_neopixel;

uint32_t dmaBuffer[NEOPIXEL_DMA_BUF_SIZE] = { 0 };	// Needs to be global, because of volatility

static void neopixel_set_color_rgb(uint8_t r, uint8_t g, uint8_t b);
static void neopixel_set_color_hsv(uint8_t h, uint8_t s, uint8_t v);

/**
 * @brief  Function implementing the tx_neopixel_app_thread_entry thread.
 * @param  thread_input: Hardcoded to 0.
 * @retval None
 */
void tx_neopixel_app_thread_entry(ULONG thread_input) {

	ULONG actual_neopixel_flags;
	UINT status;

	while (1) {

		/* Wait for neopixel event flag. */
		status = tx_event_flags_get(&event_flags_neopixel, FLAG_NEOPIXEL_ALL,
				TX_OR_CLEAR, &actual_neopixel_flags, TX_WAIT_FOREVER);

		/* Check status. */
		if (status == TX_SUCCESS) {
			switch (actual_neopixel_flags) {

			case FLAG_NEOPIXEL_OFF:
				neopixel_set_color_rgb(0, 0, 0);
				break;

			case FLAG_NEOPIXEL_RED:
				neopixel_set_color_rgb(NEOPIXEL_COLOR_VALUE, 0, 0);
				break;

			case FLAG_NEOPIXEL_GREEN:
				neopixel_set_color_rgb(0, NEOPIXEL_COLOR_VALUE, 0);
				break;

			default:
				neopixel_set_color_rgb(0, 0, 0);
				break;
			}
		}
	}
}

/**
 * @brief  Function that gets rgb input and sets it on a single neopixel led
 * @param  r: Red color value
 * @param  g: Green color value
 * @param  b: Blue color value
 * @retval None
 */
static void neopixel_set_color_rgb(uint8_t r, uint8_t g, uint8_t b) {

	PixelRGB_t pixel = { 0 };
	uint32_t *pBuff;

	pixel.color.r = r;
	pixel.color.g = g;
	pixel.color.b = b;

	pBuff = dmaBuffer;

	for (int j = 23; j >= 0; j--) {
		if ((pixel.data >> j) & 0x01) {
			*pBuff = NEOPIXEL_ONE;
		} else {
			*pBuff = NEOPIXEL_ZERO;
		}
		pBuff++;
	}

	dmaBuffer[NEOPIXEL_DMA_BUF_SIZE - 1] = 0; // last element must be 0!

	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, dmaBuffer,
	NEOPIXEL_DMA_BUF_SIZE);

}

/**
 * @brief  Function that gets hsv input and sets it on a single neopixel led
 * @param  h: Hue of the set color
 * @param  s: Saturation of the set color
 * @param  v: Value of the set color
 * @retval None
 */
static void neopixel_set_color_hsv(uint8_t h, uint8_t s, uint8_t v) {

	float r, g, b;

	h = h / 360;
	s = h / 100;
	v = h / 100;

	int i = floor(h * 6);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i % 6) {
	case 0:
		r = v, g = t, b = p;
		break;
	case 1:
		r = q, g = v, b = p;
		break;
	case 2:
		r = p, g = v, b = t;
		break;
	case 3:
		r = p, g = q, b = v;
		break;
	case 4:
		r = t, g = p, b = v;
		break;
	case 5:
		r = v, g = p, b = q;
		break;
	}

	r = r * 255;
	g = g * 255;
	b = b * 255;

	neopixel_set_color_rgb(r, g, b);

}

/**
 * @brief  PWM Pulse finished callback in non-blocking mode
 * @param  htim TIM handle
 * @retval None
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {

	HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_3); // Stops timer output after the last pulse

}
