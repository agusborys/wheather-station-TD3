#include "stdint.h"

#ifndef BME280_H_
#define BME280_H_

/**\name BME280 chip identifier */
#define BME280_CHIP_ID                            0x60

//I2C Address
#define BME280_I2C_ADD							  0x76
#define BME280_I2C_ADD2							  0x77

/**\name Register Address */
#define BME280_CHIP_ID_ADDR                       0xD0
#define BME280_RESET_ADDR                         0xE0
#define BME280_TEMP_PRESS_CALIB_DATA_ADDR         0x88
#define BME280_HUMIDITY_CALIB_DATA_ADDR           0xE1
#define BME280_PWR_CTRL_ADDR                      0xF4
#define BME280_CTRL_HUM_ADDR                      0xF2
#define BME280_CTRL_MEAS_ADDR                     0xF4
#define BME280_CONFIG_ADDR                        0xF5
#define BME280_DATA_ADDR                          0xF7
#define BME280_PRESS_ADDR                         0xF7
#define BME280_TEMP_ADDR                          0xFA
#define BME280_HUM_ADDR                           0xFD

#define BME280_CALIB_LOWERN						  26U
#define BME280_CALIB_UPPERN 					  15U
#endif /* BME280_H_ */

void reset_BME280(void);
void weatherstation_BME280(void);
void leer_BME280_Calib(void);
void leer_BME280_Raw(void);
double compensar_temp(void);
double compensar_hum(void);
double compensar_press(void);


struct BME280_calib_t {
uint16_t 	dig_T1;				// Address 0x88/0x89
int16_t 	dig_T2;				// Address 0x8A/0X8B
int16_t		dig_T3;				// Address 0x8C/0x8D
uint16_t	dig_P1;				// Address 0x8E/0x8F
int16_t		dig_P2;				// Address 0x90/0x91
int16_t		dig_P3;				// Address 0x92/0x93
int16_t		dig_P4;				// Address 0x94/0x95
int16_t		dig_P5;				// Address 0x96/0x97
int16_t		dig_P6;				// Address 0x98/0x99
int16_t		dig_P7;				// Address 0x9A/0x9B
int16_t		dig_P8;				// Address 0x9C/0x9D
int16_t		dig_P9;				// Address 0x9E/0x9F
uint8_t		dig_H1;				// Address 0xA1
int16_t		dig_H2;				// Address 0xE1/0xE2
uint8_t		dig_H3;				// Address 0xE3
int16_t		dig_H4;				// Address 0xE4/0xE5 ** mask 0b00001111
int16_t		dig_H5;				// Address 0xE5/0xE6 ** mask 0b11110000
int8_t		dig_H6;				// Address 0xE7
};


struct BME280_data_t {
int32_t	pressure_raw;
int32_t	temperature_raw;
int32_t	humidity_raw;
};



