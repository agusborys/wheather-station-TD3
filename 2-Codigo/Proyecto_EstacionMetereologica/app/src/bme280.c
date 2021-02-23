#include "bme280.h"
#include "i2c.h"

unsigned char I2C_Tx[] = {0,0,0,0};
unsigned char I2C_Rx[] = {0,0,0,0,0,0,0,0};
int success = 0;
uint8_t dataaux;
int32_t t_fine;

struct BME280_calib_t BME_Calib;
struct BME280_data_t BME_Data;


//Reseteo BME280 para "despertarlo"
void reset_BME280(void){
	I2C_Tx[0] = BME280_RESET_ADDR;
	I2C_Tx[1] = 0xB6;
	Chip_I2C_MasterSend(I2C1, BME280_I2C_ADD, I2C_Tx, 2);

	return;
}

//Pongo al BME280 en la configuración ideal para estación metereologica según el datasheet
void weatherstation_BME280(void){
	//oversampling humedad x1
	I2C_Tx[0] = BME280_CTRL_HUM_ADDR;
	I2C_Tx[1] = 0x01;
	Chip_I2C_MasterSend(I2C1, BME280_I2C_ADD, I2C_Tx, 2);
	//oversampling temp x1 - oversampling pres x1 - force mode
	I2C_Tx[0] = BME280_PWR_CTRL_ADDR;
	I2C_Tx[1] = 0x26;
	Chip_I2C_MasterSend(I2C1, BME280_I2C_ADD, I2C_Tx, 2);
	//verifico escritura correcta de registros
	I2C_Tx[0] = BME280_CTRL_HUM_ADDR;//BME280_PWR_CTRL_ADDR
	Chip_I2C_MasterSend(I2C1, BME280_I2C_ADD, I2C_Tx, 1);
	Chip_I2C_MasterRead(I2C1, BME280_I2C_ADD, I2C_Rx, 4);

	return;
}

void leer_BME280_Calib(void){
	//buffer de lectura
	uint8_t readBuff[26];
	//Leo la mitad baja de los datos de calibración
	I2C_Tx[0] = BME280_TEMP_PRESS_CALIB_DATA_ADDR;
	Chip_I2C_MasterSend(I2C1, BME280_I2C_ADD, I2C_Tx, 1);
	Chip_I2C_MasterRead(I2C1, BME280_I2C_ADD, readBuff, 26);

	//Leo el buffer y lo guardo en BME_Calib
	BME_Calib.dig_T1 = (uint16_t)((readBuff[1]*256)+readBuff[0]);
	BME_Calib.dig_T2 = (int16_t)((readBuff[3]*256)+readBuff[2]);
	BME_Calib.dig_T3 = (int16_t)((readBuff[5]*256)+readBuff[4]);

	BME_Calib.dig_P1 = (uint16_t)((readBuff[ 7]*256)+readBuff[ 6]);
	BME_Calib.dig_P2 =  (int16_t)((readBuff[ 9]*256)+readBuff[ 8]);
	BME_Calib.dig_P3 =  (int16_t)((readBuff[11]*256)+readBuff[10]);
	BME_Calib.dig_P4 =  (int16_t)((readBuff[13]*256)+readBuff[12]);
	BME_Calib.dig_P5 =  (int16_t)((readBuff[15]*256)+readBuff[14]);
	BME_Calib.dig_P6 =  (int16_t)((readBuff[17]*256)+readBuff[16]);
	BME_Calib.dig_P7 =  (int16_t)((readBuff[19]*256)+readBuff[18]);
	BME_Calib.dig_P8 =  (int16_t)((readBuff[21]*256)+readBuff[20]);
	BME_Calib.dig_P9 =  (int16_t)((readBuff[23]*256)+readBuff[22]);

	BME_Calib.dig_H1 = (uint8_t)(readBuff[24]);

	//Leo la mitad alta de los datos de calibración
	I2C_Tx[0] = BME280_HUMIDITY_CALIB_DATA_ADDR;
	Chip_I2C_MasterSend(I2C1, BME280_I2C_ADD, I2C_Tx, 1);
	Chip_I2C_MasterRead(I2C1, BME280_I2C_ADD, readBuff, 15);

	//leo el buffer y guardo en BME_Calib
	BME_Calib.dig_H2 = (int16_t)((readBuff[1]*256)+readBuff[0]);
	BME_Calib.dig_H3 = (uint8_t)(readBuff[2]);
	BME_Calib.dig_H4 = (int16_t)((readBuff[4]&0x0F)+(readBuff[3]<<4));
	BME_Calib.dig_H5 = (int16_t)(((readBuff[4]&0xF0) >>4 )  +  (readBuff[5]<<4));

	return;
}

void leer_BME280_Raw(void){
	//buffer de lectura
	uint8_t readBuff[8];
	//leo datos
	I2C_Tx[0] = BME280_DATA_ADDR;
	Chip_I2C_MasterSend(I2C1, BME280_I2C_ADD,I2C_Tx,1);
	Chip_I2C_MasterRead(I2C1, BME280_I2C_ADD,readBuff, 8);

	BME_Data.pressure_raw = (uint32_t)(
										(readBuff[0]<<12)		+
										(readBuff[1]<<4)		+
										(  (readBuff[2]&0xF0) >>4)
									   );

	BME_Data.temperature_raw = (uint32_t)(
										(readBuff[3]<<12)		+
										(readBuff[4]<<4)		+
										(  (readBuff[5]&0xF0) >>4)
									   );

	BME_Data.humidity_raw = (uint32_t)(
										(readBuff[6]<<8)		+
										(readBuff[7])
									   );

	return;
}

double compensar_temp(void){
	double var1;
	double var2;
	double temperature;
	double temperature_min = -40.0F;
	double temperature_max = 85.0F;

	var1 = ((double)BME_Data.temperature_raw) / (double)16384.0F - ((double)BME_Calib.dig_T1) / (double)1024.0F;
	var1 = var1 * ((double)BME_Calib.dig_T2);
	var2 = (((double)BME_Data.temperature_raw) / (double)131072.0F - ((double)BME_Calib.dig_T1) / (double)8192.0F);
	var2 = (var2 * var2) * ((double)BME_Calib.dig_T3);
	t_fine = (int32_t)(var1 + var2);
	temperature = (var1 + var2) / (double)5120.0F;
	if (temperature < temperature_min)
	{
		temperature = temperature_min;
	}
	else if (temperature > temperature_max)
	{
		temperature = temperature_max;
	}

	return temperature;
}

double compensar_hum(void){
	double humidity;
	double humidity_min = 0.0;
	double humidity_max = 100.0;
	double var1;
	double var2;
	double var3;
	double var4;
	double var5;
	double var6;

	var1 = ((double)t_fine) - 76800.0;
	var2 = (((double)BME_Calib.dig_H4) * 64.0 + (((double)BME_Calib.dig_H5) / 16384.0) * var1);
	var3 = BME_Data.humidity_raw - var2;
	var4 = ((double)BME_Calib.dig_H2) / 65536.0;
	var5 = (1.0 + (((double)BME_Calib.dig_H3) / 67108864.0) * var1);
	var6 = 1.0 + (((double)BME_Calib.dig_H6) / 67108864.0) * var1 * var5;
	var6 = var3 * var4 * (var5 * var6);
	humidity = var6 * (1.0 - ((double)BME_Calib.dig_H1) * var6 / 524288.0);

	if (humidity > humidity_max)
	{
		humidity = humidity_max;
	}
	else if (humidity < humidity_min)
	{
		humidity = humidity_min;
	}

	return humidity;
}

double compensar_press(void){
	double var1;
	double var2;
	double var3;
	double pressure;
	double pressure_min = 30000.0;
	double pressure_max = 110000.0;

	var1 = ((double)t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double)BME_Calib.dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)BME_Calib.dig_P5) * 2.0;
	var2 = (var2 / 4.0) + (((double)BME_Calib.dig_P4) * 65536.0);
	var3 = ((double)BME_Calib.dig_P3) * var1 * var1 / 524288.0;
	var1 = (var3 + ((double)BME_Calib.dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((double)BME_Calib.dig_P1);

	/* avoid exception caused by division by zero */
	if (var1 > (0.0))
	{
		pressure = 1048576.0 - (double) BME_Data.pressure_raw;
		pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
		var1 = ((double)BME_Calib.dig_P9) * pressure * pressure / 2147483648.0;
		var2 = pressure * ((double)BME_Calib.dig_P8) / 32768.0;
		pressure = pressure + (var1 + var2 + ((double)BME_Calib.dig_P7)) / 16.0;
		if (pressure < pressure_min)
		{
			pressure = pressure_min;
		}
		else if (pressure > pressure_max)
		{
			pressure = pressure_max;
		}
	}
	else /* Invalid case */
	{
		pressure = pressure_min;
	}

	return pressure;
}
