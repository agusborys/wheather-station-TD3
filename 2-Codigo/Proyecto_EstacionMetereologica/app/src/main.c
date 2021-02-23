
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "i2c.h"
#include "bme280.h"

// Dirección del BME280
#define SLAVE_ADDRESS	0x76

/* Buffer de escritura y escritura del i2c*/
unsigned char I2C_Buffer_Tx[] = {0,0,0,0};
unsigned char I2C_Buffer_Rx[] = {0,0,0,0,0,0,0,0};

uint8_t data = 0;
//Declaro funciones
void leerDatos(void);


/* Tarea 1 */
static void TaskBME280(void *pvParameters)
{


	//Reseteo BME280 para "despertarlo"
	reset_BME280();
	vTaskDelay(50/portTICK_RATE_MS);


	while(1)
	{
		leerDatos();
	}
}


/* Tarea 2 */
/*static void Task2(void *pvParameters)
{

	while(1)
	{

	}
}*/




/****************************************************************************************************/
/**************************************** MAIN ******************************************************/
/****************************************************************************************************/

int main(void)
{
	SystemCoreClockUpdate();
	init_i2c();
    /* Creacion de tareas */
	xTaskCreate(TaskBME280, (signed char *) "Tarea 1",
    			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
    			(xTaskHandle *) NULL);
   /* xTaskCreate(Task2, (signed char *) "Tarea 2",
        		configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
        		(xTaskHandle *) NULL);*/


    /* Start the scheduler */
	vTaskStartScheduler();

	/* Nunca debería arribar aquí */

    return 0;
}

void leerDatos(void)
{
	double Temperature;
	double Humidity;
	double Pressure;
	weatherstation_BME280();
		vTaskDelay(50/portTICK_RATE_MS);
	leer_BME280_Calib();
	leer_BME280_Raw();
	Temperature = compensar_temp();
	Humidity = compensar_hum();
	Pressure = compensar_press()/100;
	return;

}


