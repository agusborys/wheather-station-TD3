/*
 * timer.h
 *
 *  Created on: 24 feb. 2021
 *      Author: Agustin Borys
 */

#ifndef TIMER_H_
#define TIMER_H_


#include "chip.h"

#define my_TIMER 	          LPC_TIMER0
#define my_TIMER_IRQ          TIMER0_IRQn
#define my_TIMER_SPEED 		  SYSCTL_PCLK_TIMER0
#define HANDLER_TIMER_NAME    TIMER0_IRQHandler

#define MATCH0 0
#define MATCH1 1
#define MATCH2 2
#define MATCH3 3

void init_timer(void);

#endif /* TIMER_H_ */
