#include "timer.h"
#include "gsm.h"

void init_timer(void){
	uint32_t TimerFreq;

	/* Habilita el timer elegido */
	Chip_TIMER_Init(my_TIMER);

	/* Levanta la frecuencia a la que corre el clock interno del timer */
    TimerFreq = Chip_Clock_GetPeripheralClockRate(my_TIMER_SPEED);

    /* Setea el timer en modo Match e Interrupción */
	Chip_TIMER_Reset(my_TIMER);
	Chip_TIMER_MatchEnableInt(my_TIMER, MATCH0);
	Chip_TIMER_SetMatch(my_TIMER, MATCH0, (TimerFreq / TIMER_TICK_RATE_HZ));

	/* Resetea en el match */
	Chip_TIMER_ResetOnMatchEnable(my_TIMER, MATCH0);

	/* Habilita la interrupción del timer */
	NVIC_EnableIRQ(my_TIMER_IRQ);
	NVIC_ClearPendingIRQ(my_TIMER_IRQ);

	/* Arranca el timer */
	Chip_TIMER_Enable(my_TIMER);
}
