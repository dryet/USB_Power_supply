/*
 * regulator.h
 *
 *  Created on: Jan 21, 2024
 *      Author: Petr Opravil
 */

#ifndef INC_REGULATOR_H_
#define INC_REGULATOR_H_

#include "stdint.h"

#define FLAG_REGULATOR_OFF		0x01
#define FLAG_REGULATOR_ON		0x02
#define FLAG_REGULATOR_SET		0x03
#define FLAG_REGULATOR_ALL		(FLAG_REGULATOR_OFF | FLAG_REGULATOR_ON)

void regulator_enable (void);
void regulator_disable (void);
void regulator_set_voltage (uint32_t output_voltage);

#endif /* INC_REGULATOR_H_ */
