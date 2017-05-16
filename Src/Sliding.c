#include "stm32f1xx_hal.h"
#include "IO.h"
#include "Sliding.h"
#include "Adc.h"
static SlidingHandle hSliding;
void Sliding_Initilize(void)
{
	hSliding.PowerControl[0].PinId = SLIDING_PWON;
	hSliding.PowerControl[0].Init.Speed = GPIO_SPEED_HIGH;
	hSliding.PowerControl[0].Init.Mode = GPIO_MODE_OUTPUT_PP;
	hSliding.PowerControl[0].Init.Pull = GPIO_NOPULL;
	IO_Init(&hSliding.PowerControl[0]);
	IO_WritePin(&hSliding.PowerControl[0], GPIO_PIN_RESET);
	
	hSliding.PowerControl[1].PinId = SLIDING_PWON2;
	hSliding.PowerControl[1].Init.Speed = GPIO_SPEED_HIGH;
	hSliding.PowerControl[1].Init.Mode = GPIO_MODE_OUTPUT_PP;
	hSliding.PowerControl[1].Init.Pull = GPIO_NOPULL;
	IO_Init(&hSliding.PowerControl[1]);
	IO_WritePin(&hSliding.PowerControl[1], GPIO_PIN_RESET);
}
void Sliding_PowerControl(unsigned char OnOff)
{
	IO_WritePin(&hSliding.PowerControl[0], OnOff);
	IO_WritePin(&hSliding.PowerControl[1], OnOff);
}
void Sliding_PowerControl_24(unsigned char OnOff)
{
	IO_WritePin(&hSliding.PowerControl[0], OnOff);
}
void Sliding_PowerControl_5(unsigned char OnOff)
{
	IO_WritePin(&hSliding.PowerControl[1], OnOff);
}
unsigned char Sliding_PowerSate(void)
{
	return IO_ReadPinBuffer(&hSliding.PowerControl[0]);
}
