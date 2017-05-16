#include "stm32f1xx_hal.h"
#include "led.h"
#include "Io.h"
LedHandle hLed ={0};
void LED_Initilize(LED_Number LED)
{
	unsigned long const LedPinsP[] = LED_LIST(PIN_P);
	unsigned long const LedPinsN[] = LED_LIST(PIN_N);
	
	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	hLed.Led[LED].PinP.PinId = LedPinsP[LED];
	hLed.Led[LED].PinN.PinId = LedPinsN[LED];
	
	hLed.Led[LED].PinP.Init.Speed = hLed.Led[LED].PinN.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	hLed.Led[LED].PinP.Init.Mode = hLed.Led[LED].PinN.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hLed.Led[LED].PinP.Init.Speed = hLed.Led[LED].PinN.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	
	IO_Init(&hLed.Led[LED].PinP);
	IO_Init(&hLed.Led[LED].PinN);
	
	LED_OnOff(LED, 0);

}
void LED_OnOff(LED_Number LED, unsigned char On)
{
	IO_WritePin(&hLed.Led[LED].PinP, On);
	IO_WritePin(&hLed.Led[LED].PinN, !On);
}
void LED_Toggle(LED_Number LED)
{
	IO_TogglePin(&hLed.Led[LED].PinP);
	IO_TogglePin(&hLed.Led[LED].PinN);
}
LED_DRIVER_DEF(1)
LED_Driver *LEDS[] = {&LED1_Driver};
