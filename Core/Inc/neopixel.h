/*
 * neopixel.h
 *
 *  Created on: Jan 20, 2024
 *      Author: Petr Opravil
 */

#ifndef INC_NEOPIXEL_H_
#define INC_NEOPIXEL_H_

#include "main.h"
#include "tx_api.h"

#define NEOPIXEL_ZERO			25
#define NEOPIXEL_ONE			51
#define NEOPIXEL_NUM			1
#define NEOPIXEL_DMA_BUF_SIZE	((NEOPIXEL_NUM*24) + 1)
#define NEOPIXEL_COLOR_VALUE	50

#define FLAG_NEOPIXEL_OFF		0x01
#define FLAG_NEOPIXEL_RED		0x02
#define FLAG_NEOPIXEL_GREEN		0x03
#define FLAG_NEOPIXEL_ALL		(FLAG_NEOPIXEL_OFF | FLAG_NEOPIXEL_RED | FLAG_NEOPIXEL_GREEN)

typedef union {
	struct {
		uint8_t b;
		uint8_t r;
		uint8_t g;
	} color;
	uint32_t data;
} PixelRGB_t;

void tx_neopixel_app_thread_entry(ULONG thread_input);

#endif /* INC_NEOPIXEL_H_ */
