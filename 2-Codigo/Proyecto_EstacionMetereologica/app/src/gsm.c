#include "gsm.h"
#include "chip.h"
#include "timer.h"
#include "string.h"
#include <stdio.h>

STATIC RINGBUFF_T txring, rxring;

static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

//Comandos AT para comunicarse con SIM800L
//Comandos de config
const char comm1[] = "AT\r\n";
const char comm2[] = "AT+IPR=9600\r\n";

//Comandos configuracion de GPRS
const char commGPRS[] = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
const char commApnM[] = "AT+SAPBR=3,1,\"APN\",\"wap.gprs.unifon.com.ar\"\r\n";
const char commUserM[] = "AT+SAPBR=3,1,\"USER\",\"wap\"\r\n";
const char commPwdM[] = "AT+SAPBR=3,1,\"PWD\",\"wap\"\r\n";
const char commOpenGprs[] = "AT+SAPBR=1,1\r\n";
const char commApnComplete[] = "AT+CSTT=\"wap.gprs.unifon.com.ar\",\"wap\",\"wap\"\r\n";
const char commCheckIP[] = "AT+SAPBR=2,1\r\n";
const char commAttGprs[] = "AT+CGATT=1\r\n";
const char commApnC[] = "AT+SAPBR=3,1,\"APN\",\"igprs.claro.com.ar\"\r\n";
const char commUserC[] = "AT+SAPBR=3,1,\"USER\",\"clarogprs\"\r\n";
const char commPwdC[] = "AT+SAPBR=3,1,\"PWD\",\"clarogprs999\"\r\n";

//Comandos inicialización HTTP
const char commHttpInit[] = "AT+HTTPINIT\r\n";
const char commHttpProfile[] = "AT+HTTPPARA=\"CID\",1\r\n";
const char commHttpUrl[] = "AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=CVMAU399YCLAT9TH\"";
const char commHttpAction[] = "AT+HTTPACTION=0\r\n";
const char commHttpFinish[] = "AT+HTTPTERM\r\n";
const char commHttpRead[] = "AT+HTTPREAD\r\n";

//Variables a utilizar
uint8_t check[100];
uint8_t key;
int bytes;
int i = 0;
int a = 0;
char data[150];
char* fieldTemp = "&field1=";
char* fieldHum = "&field2=";
char* fieldPress = "&field3=";
char* fieldLum = "&field4=";
char* fieldHumSuelo = "&field5=";

void init_uart(void){

	Chip_IOCON_PinMuxSet(LPC_IOCON,0,10,FUNC1);   //Asigno el pin P0.10 como TXD2
	Chip_IOCON_PinMuxSet(LPC_IOCON,0,11,FUNC1);   //Asigno el pin P0.11 como RXD2

	//configuracion del uart
	Chip_UART_Init(UART_SELECTION);
	Chip_UART_SetBaud(UART_SELECTION, 9600);
	Chip_UART_ConfigData(UART_SELECTION, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(UART_SELECTION, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(UART_SELECTION);

	//inicializa el buffer
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	Chip_UART_SetupFIFOS(UART_SELECTION, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
							UART_FCR_TX_RS | UART_FCR_TRG_LEV3));
	Chip_UART_IntEnable(UART_SELECTION, (UART_IER_RBRINT | UART_IER_RLSINT));


	NVIC_SetPriority(IRQ_SELECTION, 1);
	NVIC_EnableIRQ(IRQ_SELECTION);
}

void HANDLER_NAME(void){
	Chip_UART_IRQRBHandler(UART_SELECTION, &rxring, &txring);
}

void HANDLER_TIMER_NAME(void){

	static uint32_t value=0;

	if (Chip_TIMER_MatchPending(my_TIMER, MATCH0))
	{
		Chip_TIMER_ClearMatch(my_TIMER, MATCH0);

		if  (value < 500)
			value++;
		else{
			value=0;
			a=1;

		}
	}
}

int SerialDisponible(void){
	bytes = Chip_UART_ReadRB(UART_SELECTION, &rxring, &key, 1); //Guardo el dato en key
	if (bytes > 0)
	return 1;
	else
	return 0;
}

void init_gsm(void){
	Chip_UART_SendRB(UART_SELECTION, &txring, &comm1, sizeof(comm1)-1);
	//Hago demora para recibir respuesta
	waitForOk();
	Chip_UART_SendRB(UART_SELECTION, &txring, &comm2, sizeof(comm2) - 1);
	waitForOk();


}

void SendData(char* temp, char* hum, char* press,char* lum,char* humSuelo){

	strcat(data,commHttpUrl);
	strcat(data,fieldTemp);
	strcat(data,temp);
	strcat(data,fieldHum);
	strcat(data,hum);
	strcat(data,fieldPress);
	strcat(data,press);
	strcat(data,fieldLum);
	strcat(data,lum);
	strcat(data,fieldHumSuelo);
	strcat(data,humSuelo);
	strcat(data,"\"\r\n");


//	Chip_UART_SendRB(UART_SELECTION, &txring, &commAttGprs, sizeof(commAttGprs) - 1);
//	waitForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commGPRS, sizeof(commGPRS) - 1);
	waitForOk();

//	Chip_UART_SendRB(UART_SELECTION, &txring, &commApnComplete, sizeof(commApnComplete) - 1);
//	waitForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commApnC, sizeof(commApnC) - 1);
	waitForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commUserC, sizeof(commUserC) - 1);
	waitForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commPwdC, sizeof(commPwdC) - 1);
	waitForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commOpenGprs, sizeof(commOpenGprs) - 1);
	waitLongForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commCheckIP, sizeof(commCheckIP) - 1);
	waitForOk();

	httpConection();

}


void httpConection(void){
	Chip_UART_SendRB(UART_SELECTION, &txring, &commHttpInit, sizeof(commHttpInit) - 1);
	waitForOk();
//	a=0;
//	while(a==0);

	Chip_UART_SendRB(UART_SELECTION, &txring, &commHttpProfile, sizeof(commHttpProfile) - 1);
	waitLongForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &data, sizeof(data) - 1);
	waitForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commHttpAction, sizeof(commHttpAction) - 1);
	waitForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commHttpRead, sizeof(commHttpRead) - 1);
	waitForOk();

	Chip_UART_SendRB(UART_SELECTION, &txring, &commHttpFinish, sizeof(commHttpFinish) - 1);
	waitForOk();
}

void waitForOk(void){

	for(i = 0;i<5;i++){
		a = 0;
		while(a==0);
	}
	i = 0;
	while (SerialDisponible()){
		check[i] = key;
		i++;
	}
}
void waitLongForOk(void){

	for(i = 0;i<15;i++){
		a = 0;
		while(a==0);
	}
	i = 0;
	while (SerialDisponible()){
		check[i] = key;
		i++;
	}
}

