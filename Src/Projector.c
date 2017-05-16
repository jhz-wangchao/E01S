#include "stm32f1xx_hal.h"
#include "IO.h"
#include "Projector.h"
#include "Adc.h"
static ProjectorHandle hPr;
static void ProjectorUpDect(void);
static void ProjectorDownDect(void);
void Projector_Initilize(void)
{
	hPr.Dir = hPr.Status = hPr.TimeCount = 0;
	
	hPr.TimeBaseHandle.Instance = PROJECTOR_TIMX;
	hPr.TimeBaseHandle.Init.Period = 1000;
	hPr.TimeBaseHandle.Init.Prescaler = HAL_RCC_GetPCLK2Freq()/1000000 - 1;
	hPr.TimeBaseHandle.Init.ClockDivision = 0;
	hPr.TimeBaseHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	PROJECTOR_CLOCK_ENABLE();
	HAL_TIM_Base_Init(&hPr.TimeBaseHandle);
	
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	HAL_NVIC_SetPriority(PROJECTOR_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(PROJECTOR_IRQn);
	HAL_TIM_Base_Stop(&hPr.TimeBaseHandle);
	
	hPr.UpControl.PinId = PROJECTOR_UP_CTRL;
	hPr.UpControl.Init.Speed = GPIO_SPEED_HIGH;
	hPr.UpControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPr.UpControl.Init.Pull = GPIO_NOPULL;
	IO_Init(&hPr.UpControl);
	IO_WritePin(&hPr.UpControl, GPIO_PIN_RESET);
	
	hPr.DownControl.PinId = PROJECTOR_DOWN_CTRL;
	hPr.DownControl.Init.Speed = GPIO_SPEED_HIGH;
	hPr.DownControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPr.DownControl.Init.Pull = GPIO_NOPULL;
	IO_Init(&hPr.DownControl);
	IO_WritePin(&hPr.DownControl, GPIO_PIN_RESET);
	
	hPr.PowerControl.PinId = PROJECTOR_PWON;
	hPr.PowerControl.Init.Speed = GPIO_SPEED_HIGH;
	hPr.PowerControl.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hPr.PowerControl.Init.Pull = GPIO_NOPULL;
	IO_Init(&hPr.PowerControl);
	IO_WritePin(&hPr.PowerControl, GPIO_PIN_RESET);
	
	hPr.UpDecete.PinId = PROJECTOR_UP_DECETE;
	hPr.UpDecete.Init.Speed = GPIO_SPEED_HIGH;
	hPr.UpDecete.Init.Mode = GPIO_MODE_IT_FALLING;
	hPr.UpDecete.Init.Pull = GPIO_PULLUP;
	hPr.UpDecete.ExtiIrqCallback = ProjectorUpDect;
	IO_Init(&hPr.UpDecete);
	
	hPr.DownDecete.PinId = PROJECTOR_DOWN_DECETE;
	hPr.DownDecete.Init.Speed = GPIO_SPEED_HIGH;
	hPr.DownDecete.Init.Mode = GPIO_MODE_IT_FALLING;
	hPr.DownDecete.Init.Pull = GPIO_PULLUP;
	hPr.DownDecete.ExtiIrqCallback = ProjectorDownDect;
	IO_Init(&hPr.DownDecete);
	
}
void PROJECTOR_IRQHandler()
{
	HAL_TIM_IRQHandler(&hPr.TimeBaseHandle);
}
static void ProjectorUpDect(void)
{
	HAL_TIM_Base_Stop(&hPr.TimeBaseHandle);
	IO_WritePin(&hPr.UpControl, 0);
	hPr.Status = PROJECT_DOUR_OPEN;
}
static void ProjectorDownDect(void)
{
	HAL_TIM_Base_Stop(&hPr.TimeBaseHandle);
	IO_WritePin(&hPr.DownControl, 0);
	hPr.Status = PROJECT_DOUR_CLOSE;
}
void HAL_TIM_PeriodElapsedCallbackNext(TIM_HandleTypeDef *htim)
{
	if(&hPr.TimeBaseHandle == htim)
	{
		hPr.TimeCount++;
		if(hPr.TimeCount >= 20000)
		{
			IO_WritePin(&hPr.UpControl, 0);
			IO_WritePin(&hPr.DownControl, 0);
			hPr.Status = PROJECT_DOUR_TIMEOUT;
			HAL_TIM_Base_Stop(&hPr.TimeBaseHandle);
		}
		else
		{
			if(hPr.Dir)
			{
				IO_TogglePin(&hPr.UpControl);
			}
			else
			{
				IO_TogglePin(&hPr.DownControl);
			}
		}
	}
}
void Projector_PowerControl(unsigned char OnOff)
{
	IO_WritePin(&hPr.PowerControl, OnOff);
}
unsigned char Projector_PowerSate(void)
{
	return IO_ReadPinBuffer(&hPr.PowerControl);
}
void Projector_Move(unsigned int Dir)
{
	if(IO_ReadPin(&hPr.DownDecete) && !Dir)
	{
		hPr.Status = PROJECT_DOUR_CLOSE;
		return;
	}
	if(IO_ReadPin(&hPr.UpDecete) && Dir)
	{
		hPr.Status = PROJECT_DOUR_OPEN;
		return;
	}
	hPr.Dir = Dir;
	hPr.TimeCount = 0;
	hPr.Status = PROJECT_DOUR_MOVEING;
	HAL_TIM_Base_Start_IT(&hPr.TimeBaseHandle);
}

int Projector_DourStatus(void)
{
	return hPr.Status;
}
int Projector_UGet(void)
{
	int U = ADC_Read(7);
	U *= 9075;
	U >>= 10;
	return U;
}
int Projector_IGet(void)
{
	int I = ADC_Read(14);
	I *= 4125;
	I /= 17;
	I >>= 7;
	return I;
}
