
#ifndef GSM_H_
#define GSM_H_

#define UART_SELECTION 	LPC_UART2
#define IRQ_SELECTION 	UART2_IRQn
#define HANDLER_NAME 	UART2_IRQHandler

#define MAX_VALUE 1000 //Valor máximo de la rampa (<1024)
#define TIMER_TICK_RATE_HZ 1000 //Tick para actualizar el valor de la rampa
#define STEP 1
#define THRESHOLD 500

#define UART_SRB_SIZE 256	/* Send */
#define UART_RRB_SIZE 256	/* Receive */

void init_uart(void);
void init_timer(void);
void init_gsm(void);
void HANDLER_TIMER_NAME(void);
void HANDLER_NAME(void);
int SerialDisponible(void);
void setAPN(char* temp, char* hum, char* press);
void waitForOk(void);
void sendDataHttp(double temp, double hum, double press);
void httpConection(void);

#endif /* GSM_H_ */
