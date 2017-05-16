#include "stm32f1xx_hal.h"
#include "IO.h"
#include "Power.h"
#include "Log.h"


struct
{
	IO_HandleTypeDef Sos;
	IO_HandleTypeDef Detect;
	IO_HandleTypeDef On;
	IO_HandleTypeDef Off;
}hPower;

void Power_Initilize(void)
{
	hPower.Sos.PinId = POWER_KEY_SOS;
	hPower.Sos.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	hPower.Sos.Init.Mode = GPIO_MODE_INPUT;
	hPower.Sos.Init.Pull = GPIO_PULLUP;
	IO_Init(&hPower.Sos);
	
	hPower.Detect.PinId = POWER_KEY_DETECT;
	hPower.Detect.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	hPower.Detect.Init.Mode = GPIO_MODE_INPUT;
	hPower.Detect.Init.Pull = GPIO_PULLUP;
	IO_Init(&hPower.Detect);
	
	hPower.On.PinId = POWER_ON_CONTROL;
	hPower.On.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	hPower.On.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPower.On.Init.Pull = GPIO_PULLUP;
	IO_Init(&hPower.On);
	IO_WritePin(&hPower.On, 1);
	
	hPower.Off.PinId = POWER_OFF_CONTROL;
	hPower.Off.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	hPower.Off.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPower.Off.Init.Pull = GPIO_PULLDOWN;
	IO_Init(&hPower.Off);
	IO_WritePin(&hPower.Off, 0);
	
	Log::Infomation("Power On");
}

unsigned char Power_KeyDetect(void)
{
	return IO_ReadPin(&hPower.Detect);
}
unsigned char Power_SosDetect(void)
{
	return IO_ReadPin(&hPower.Sos);
}
void Power_Off(void)
{
	IO_WritePin(&hPower.On, 0);
	HAL_PWR_EnterSTANDBYMode();
}

