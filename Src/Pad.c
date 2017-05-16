#include "stm32f1xx_hal.h"
#include "IO.h"
#include "Pad.h"
#include "Adc.h"
static PadHandle hPad;
void Pad_Initilize(void)
{
	hPad.PowerControl.PinId = PAD_PWON;
	hPad.PowerControl.Init.Speed = GPIO_SPEED_HIGH;
	hPad.PowerControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPad.PowerControl.Init.Pull = GPIO_NOPULL;
	IO_Init(&hPad.PowerControl);
	IO_WritePin(&hPad.PowerControl, GPIO_PIN_RESET);
	
	hPad.AudioControl.PinId = PAD_AUDIO_CTRL;
	hPad.AudioControl.Init.Speed = GPIO_SPEED_HIGH;
	hPad.AudioControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPad.AudioControl.Init.Pull = GPIO_NOPULL;
	IO_Init(&hPad.AudioControl);
	IO_WritePin(&hPad.AudioControl, GPIO_PIN_RESET);
	
	hPad.StopControl.PinId = PAD_STOP;
	hPad.StopControl.Init.Speed = GPIO_SPEED_HIGH;
	hPad.StopControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPad.StopControl.Init.Pull = GPIO_PULLDOWN;
	IO_Init(&hPad.StopControl);
	IO_WritePin(&hPad.StopControl, GPIO_PIN_RESET);
	
	hPad.BeepControl.PinId = BEEP_CONTROL;
	hPad.BeepControl.Init.Speed = GPIO_SPEED_HIGH;
	hPad.BeepControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPad.BeepControl.Init.Pull = GPIO_PULLDOWN;
	IO_Init(&hPad.BeepControl);
	IO_WritePin(&hPad.BeepControl, GPIO_PIN_RESET);
}
void Pad_PowerControl(unsigned char OnOff)
{
	IO_WritePin(&hPad.PowerControl, OnOff);
}
unsigned char Pad_PowerSate(void)
{
	return IO_ReadPinBuffer(&hPad.PowerControl);
}
void Pad_AudioControl(unsigned char OnOff)
{
	IO_WritePin(&hPad.AudioControl, OnOff);
}
unsigned char Pad_AudioSate(void)
{
	return IO_ReadPinBuffer(&hPad.AudioControl);
}
void Pad_BeepControl(unsigned char OnOff)
{
	IO_WritePin(&hPad.BeepControl, OnOff);
}
