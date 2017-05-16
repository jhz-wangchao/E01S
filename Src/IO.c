#include "stm32f1xx_hal.h"
#include "IO.h"
#define GPIO_MODE_IT          ((uint32_t)0x00010000)
static const IRQn_Type ExitIrqs[] ={EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn, 
							EXTI9_5_IRQn,EXTI9_5_IRQn,EXTI9_5_IRQn,EXTI9_5_IRQn, EXTI9_5_IRQn,
							EXTI15_10_IRQn,EXTI15_10_IRQn,EXTI15_10_IRQn,EXTI15_10_IRQn,EXTI15_10_IRQn,EXTI15_10_IRQn};
static void IrqIdle(){};
static ExtiIrqCallbackDelegate ExitIrqCallback[]={IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle,IrqIdle};
					
static unsigned char gpio_portId(PortBaseAddr *GPIOx)
{
	return ((unsigned long)GPIOx - (unsigned long)GPIOA) / ((unsigned long)GPIOB - (unsigned long)GPIOA);
}
void GPIO_PortClock (PortBaseAddr *GPIOx, unsigned char enable) 
{
	if(enable)
	{
		RCC->APB2ENR |= (1U << (2 + gpio_portId(GPIOx)));
	}
	else
	{
		RCC->APB2ENR &= ~(1U << (2 + gpio_portId(GPIOx)));
	}
}

unsigned char GPIO_GetPortClockState (PortBaseAddr *GPIOx)
{
	return ((RCC->APB2ENR &  (1U << (2 + gpio_portId(GPIOx)))) != 0U);
}
void IO_Init(pIO_HandleTypeDef hIo)
{
	int i;
	hIo->Instance = (GPIO_TypeDef *)((unsigned long)GPIOA + ((unsigned long)GPIOB - (unsigned long)GPIOA) * (hIo->PinId>>IO_PORT_SHIFT));
	hIo->Pin = (hIo->PinId) & 0xffff;
		
	hIo->Init.Pin = hIo->Pin;
	if(hIo->Pin)
	{
		GPIO_PortClock(hIo->Instance, ENABLE);
		if(GPIO_MODE_OUTPUT_PP == hIo->Init.Mode)
		{
			if(GPIO_PULLUP == hIo->Init.Pull)
			{
				IO_WritePin(hIo, 1);
			}
			else if(GPIO_PULLDOWN == hIo->Init.Pull)
			{
				IO_WritePin(hIo, 0);
			}
		}
		HAL_GPIO_Init(hIo->Instance, &hIo->Init);
		if((PinCode(PORTA, 1<<15) == hIo->PinId) || (PinCode(PORTB, 1<<3) == hIo->PinId) || (PinCode(PORTB, 1<<4) == hIo->PinId))
		{
			__HAL_RCC_AFIO_CLK_ENABLE();
			__HAL_AFIO_REMAP_SWJ_NOJTAG();
			
		}
		if(hIo->Init.Mode & GPIO_MODE_IT)
		{
			for(i=0; i<16;i++)
			{
				if(hIo->Pin & (1<<i))
				{
					HAL_NVIC_SetPriority(ExitIrqs[i], 3, 0);
					HAL_NVIC_EnableIRQ(ExitIrqs[i]);
					if(hIo->ExtiIrqCallback != NULL)
					{
						ExitIrqCallback[i] = hIo->ExtiIrqCallback;
					}
				}
			}
			
		}
	}
}
void IO_DeInit(pIO_HandleTypeDef hIo)
{
	HAL_GPIO_DeInit(hIo->Instance, hIo->Pin);
}
unsigned char IO_ReadPin(pIO_HandleTypeDef hIo)
{
	if(hIo->Pin)
	{
		return (unsigned char)HAL_GPIO_ReadPin(hIo->Instance, hIo->Pin);
	}
	return 0;
}
void IO_WritePin(pIO_HandleTypeDef hIo, unsigned char PinState)
{
	if(hIo->Pin)
	{
		HAL_GPIO_WritePin(hIo->Instance, hIo->Pin, (GPIO_PinState)PinState);
	}
}
unsigned char IO_ReadPinBuffer(pIO_HandleTypeDef hIo)
{
	if(hIo->Pin)
	{
		if(hIo->Instance->ODR & hIo->Pin)
		{
			return 1;
		}
	}
	return 0;
}
void  IO_TogglePin(pIO_HandleTypeDef hIo)
{
	if(hIo->Pin)
	{
		HAL_GPIO_TogglePin(hIo->Instance, hIo->Pin);
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	int i;
	for(i=0; i<16;i++)
	{
		if(GPIO_Pin & (1<<i))
		{
			if(ExitIrqCallback[i])
			{
				ExitIrqCallback[i]();
			}
		}
	}
}
void EXTI0_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(1<<0);
}
void EXTI1_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(1<<1);
}
void EXTI2_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(1<<2);
}
void EXTI3_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(1<<3);
}
void EXTI4_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(1<<4);
}
void EXTI9_5_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(EXTI->PR & (0x03E0));
}
void EXTI15_10_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(EXTI->PR & (0xFC00));
}
