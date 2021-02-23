/*
 * i2c.h
 *
 *  Created on: 12 feb. 2021
 *      Author: Agustin Borys
 */

#ifndef I2C_H_
#define I2C_H_

#include "chip.h"

#define SDA0_PORT  0
#define SDA0_PIN  27
#define SCL0_PORT  0
#define SCL0_PIN  28

#define SDA1_PORT  0
#define SDA1_PIN   0
#define SCL1_PORT  0
#define SCL1_PIN   1

#define SDA2_PORT  0
#define SDA2_PIN  10
#define SCL2_PORT  0
#define SCL2_PIN  11

void init_i2c(void);

#endif /* I2C_H_ */
