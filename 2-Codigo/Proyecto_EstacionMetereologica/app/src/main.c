
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "i2c.h"
#include "bme280.h"
#include "gsm.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

//Declaro funciones
void leerDatos(void);

double Temperature;
double Humidity;
double Pressure;

char tempChar[6];
char humChar[6];
char pressChar[8];
/* Tarea 1 */
static void TaskBME280(void *pvParameters)
{


	//Reseteo BME280 para "despertarlo"
	reset_BME280();
	vTaskDelay(50/portTICK_RATE_MS);


	while(1)
	{
		leerDatos();
		sendDataHttp(Temperature, Humidity, Pressure);
	}
}


/* Tarea 2 */
static void TaskGSM(void *pvParameters)
{
	reset_BME280();
	vTaskDelay(50/portTICK_RATE_MS);
	init_gsm();
	leerDatos();

	//httpComunication();
	while(1)
	{
		leerDatos();
		setAPN(tempChar, humChar, pressChar);
		//sendDataHttp(Temperature, Humidity, Pressure);
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
    /* Creacion de tareas */
//	xTaskCreate(TaskBME280, (signed char *) "Tarea 1",
//    			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
//    			(xTaskHandle *) NULL);
    xTaskCreate(TaskGSM, (signed char *) "Tarea 2",
        		configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
        		(xTaskHandle *) NULL);


    /* Start the scheduler */
	vTaskStartScheduler();

	/* Nunca debería arribar aquí */

    return 0;
}

void leerDatos(void)
{
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


