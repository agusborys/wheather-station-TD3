
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"


/* Tarea 1 */
static void Task1(void *pvParameters)
{

	while(1)
	{

	}
}


/* Tarea 2 */
static void Task2(void *pvParameters)
{

	while(1)
	{

	}
}




/****************************************************************************************************/
/**************************************** MAIN ******************************************************/
/****************************************************************************************************/

int main(void)
{
	SystemCoreClockUpdate();

    /* Creacion de tareas */
	xTaskCreate(Task1, (signed char *) "Tarea 1",
    			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
    			(xTaskHandle *) NULL);
    xTaskCreate(Task2, (signed char *) "Tarea 2",
        		configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
        		(xTaskHandle *) NULL);


    /* Start the scheduler */
	vTaskStartScheduler();

	/* Nunca debería arribar aquí */

    return 0;
}

