
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "i2c.h"
#include "bme280.h"
#include "gsm.h"
#include "timer.h"
#include "adc.h"
#include <stdio.h>
#include "string.h"

//Declaro funciones
void leerDatos(void);
void leerADCs(void);

//Variables Double
double Temperature;
double Humidity;
double Pressure;
double Luminosity;
double GroundHumidity;
double coefADC_Porc = 100.0/4096.0;

//Variables int16
uint16_t dataLDR;
uint16_t dataGnd;

//variables Char
int flagADC=0;
char tempChar[6];
char humChar[6];
char pressChar[8];
char lumChar[6];
char humSueloChar[6];

//Handler de semaforos
xSemaphoreHandle s1;
xSemaphoreHandle s2;
xSemaphoreHandle s3;

/* Tarea 1 */
static void TaskBME280(void *pvParameters)
{

	//Reseteo BME280 para "despertarlo"
	reset_BME280();
	//vTaskDelay(50/portTICK_RATE_MS);

	while(1)
	{
		xSemaphoreTake(s1,portMAX_DELAY);

		leerDatos();

		xSemaphoreGive(s3);

	}
}


/* Tarea 2 */
static void TaskGSM(void *pvParameters)
{

	init_gsm();

	while(1)
	{
		xSemaphoreTake(s2,portMAX_DELAY);

		SendData(tempChar, humChar, pressChar, lumChar, humSueloChar);

		xSemaphoreGive(s1);
	}
}

/* Tarea 3 */
static void TaskLDR(void *pvParameters)
{
	init_adc();

	while(1)
	{
		xSemaphoreTake(s3,portMAX_DELAY);

		leerADCs();

		xSemaphoreGive(s2);

	}
}


/****************************************************************************************************/
/**************************************** MAIN ******************************************************/
/****************************************************************************************************/

int main(void)
{
	SystemCoreClockUpdate();
	init_i2c();
	init_timer();
	init_uart();

/* Creación de los semáforos */
	vSemaphoreCreateBinary(s1);
	vSemaphoreCreateBinary(s2);
	vSemaphoreCreateBinary(s3);

/* Condición inicial: s2 tomado, s1 y s3 liberado */
		xSemaphoreTake(s2,0);

/* Creacion de tareas */
	xTaskCreate(TaskBME280, (signed char *) "Tarea 1",
    			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
    			(xTaskHandle *) NULL);
    xTaskCreate(TaskGSM, (signed char *) "Tarea 2",
        		configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
        		(xTaskHandle *) NULL);
    xTaskCreate(TaskLDR, (signed char *) "Tarea 3",
            		configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
            		(xTaskHandle *) NULL);


    /* Start the scheduler */
	vTaskStartScheduler();

	/* Nunca debería arribar aquí */

    return 0;
}

void leerDatos(void){
	weatherstation_BME280();
	vTaskDelay(50/portTICK_RATE_MS);
	leer_BME280_Calib();
	leer_BME280_Raw();
	Temperature = compensar_temp();
	Humidity = compensar_hum();
	Pressure = compensar_press()/100;
	sprintf(tempChar,"%.2f",Temperature);
	sprintf(humChar,"%.2f",Humidity);
	sprintf(pressChar,"%.2f",Pressure);

	return;

}

void leerADCs(void){

	Luminosity = 100 - (dataLDR * coefADC_Porc);
	GroundHumidity = 100 - (dataGnd * coefADC_Porc);
	sprintf(humSueloChar,"%.2f",GroundHumidity);
	sprintf(lumChar,"%.2f",Luminosity);


}

void ADC_IRQHandler(void)
{

	if(flagADC==0){
		/* Lee el valor del ADC y lo guarda en dataADC */
		Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, &dataLDR);
		Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, DISABLE);
		Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
		Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH0, DISABLE);
		Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH2, ENABLE);
		Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		flagADC = 1;
		return;
	}

	if(flagADC==1){
			/* Lee el valor del ADC y lo guarda en dataADC */
		Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &dataGnd);
		Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
		Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, DISABLE);
		Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH0, ENABLE);
		Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH2, DISABLE);
		Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		flagADC = 0;
		return;
		}

	return;
}

