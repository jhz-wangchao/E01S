#include "stm32f1xx_hal.h"
#include "IO.h"
#include "Ir.h"
#include "Adc.h"
static IrHandle hIr;
void Ir_Initilize(void)
{
	hIr.PowerControl.PinId = IR_PWON;
	hIr.PowerControl.Init.Speed = GPIO_SPEED_HIGH;
	hIr.PowerControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hIr.PowerControl.Init.Pull = GPIO_NOPULL;
	IO_Init(&hIr.PowerControl);
	IO_WritePin(&hIr.PowerControl, GPIO_PIN_RESET);
	
	hIr.PowerOverload.PinId = IR_PWOV;
	hIr.PowerOverload.Init.Speed = GPIO_SPEED_HIGH;
	hIr.PowerOverload.Init.Mode = GPIO_MODE_INPUT;
	hIr.PowerOverload.Init.Pull = GPIO_PULLDOWN;
	IO_Init(&hIr.PowerControl);
	
}
void Ir_PowerControl(unsigned char OnOff)
{
	IO_WritePin(&hIr.PowerControl, OnOff);
}
unsigned char Ir_PowerSate(void)
{
	return IO_ReadPinBuffer(&hIr.PowerControl);
}
unsigned char Ir_PowerOverload(void)
{
	return IO_ReadPin(&hIr.PowerOverload);
}
