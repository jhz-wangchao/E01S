#include "stm32f1xx_hal.h"
#include "IO.h"
#include "UltraSound.h"
#include "Adc.h"
static UltraSoundHandle hUltraSound;
void UltraSound_Initilize(void)
{
	hUltraSound.PowerControl.PinId = ULTRASOUND_PWON;
	hUltraSound.PowerControl.Init.Speed = GPIO_SPEED_HIGH;
	hUltraSound.PowerControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hUltraSound.PowerControl.Init.Pull = GPIO_NOPULL;
	IO_Init(&hUltraSound.PowerControl);
	IO_WritePin(&hUltraSound.PowerControl, GPIO_PIN_RESET);
	
	hUltraSound.PowerOverload.PinId = ULTRASOUND_PWOV;
	hUltraSound.PowerOverload.Init.Speed = GPIO_SPEED_HIGH;
	hUltraSound.PowerOverload.Init.Mode = GPIO_MODE_INPUT;
	hUltraSound.PowerOverload.Init.Pull = GPIO_PULLDOWN;
	IO_Init(&hUltraSound.PowerControl);
	
}
void UltraSound_PowerControl(unsigned char OnOff)
{
	IO_WritePin(&hUltraSound.PowerControl, OnOff);
}
unsigned char UltraSound_PowerSate(void)
{
	return IO_ReadPinBuffer(&hUltraSound.PowerControl);
}
unsigned char UltraSound_PowerOverload(void)
{
	return IO_ReadPin(&hUltraSound.PowerOverload);
}
