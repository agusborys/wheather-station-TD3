#include <i2c.h>


void I2C1_IRQHandler(void)
{
	Chip_I2C_MasterStateHandler(I2C1);
}


void init_i2c(void){

	/* Configuración de los pines. La EEPROM está conectada al puerto I2C1. */
	Chip_IOCON_PinMux(LPC_IOCON, SDA1_PORT, SDA1_PIN, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_PinMux(LPC_IOCON, SCL1_PORT, SCL1_PIN, IOCON_MODE_INACT, IOCON_FUNC3);

	/* Importante habilitar OPEN DRAIN */
	Chip_IOCON_EnableOD(LPC_IOCON, SDA1_PORT, SDA1_PIN);
	Chip_IOCON_EnableOD(LPC_IOCON, SCL1_PORT, SCL1_PIN);

	/* Habilita el módulo */
	Chip_I2C_Init(I2C1);

	/* Establece el clock */
	Chip_I2C_SetClockRate(I2C1, 100000);

	/* Handler de I2C Modo Master */
	Chip_I2C_SetMasterEventHandler(I2C1, Chip_I2C_EventHandler);

	/* Habilita la interrupción */
	NVIC_EnableIRQ(I2C1_IRQn);


}
