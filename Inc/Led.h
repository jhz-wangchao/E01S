#ifndef __LED_H__
#define __LED_H__
#ifdef __cplusplus
extern "C"{
#endif
	
typedef enum
{
	LED1,
	LEDNUM
}LED_Number;

#ifdef __STM32F1xx_HAL_H
#include "Io.h"
#define LED1_PIN_P		PinCode(PORTB, GPIO_PIN_3)
#define LED1_PIN_N		PinCode(PORTB, GPIO_PIN_4)

#define LED_LIST(W)		{LED1_##W}

typedef struct
{
	struct
	{
		IO_HandleTypeDef PinP;
		IO_HandleTypeDef PinN;
	}Led[LEDNUM];
}LedHandle, *pLedHandle;
#endif

void LED_Initilize(LED_Number LED);
void LED_OnOff(LED_Number LED, unsigned char On);
void LED_Toggle(LED_Number LED);

typedef struct
{
	void (*Initilize)(void);
	void (*OnOff)(unsigned char On);
	void (*Toggle)(void);
}LED_Driver;

#define LED_DRIVER_DEF(A)	\
	void LED##A##_Initilize(void){LED_Initilize(LED##A##);} \
	void LED##A##_OnOff(unsigned char On){LED_OnOff(LED##A##, On);} \
	void LED##A##_Toggle(void){LED_Toggle(LED##A##);} \
	LED_Driver LED##A##_Driver = {LED##A##_Initilize, LED##A##_OnOff, LED##A##_Toggle};

extern LED_Driver LED1_Driver;
extern LED_Driver *LEDS[];
#ifdef __cplusplus
}
#endif
#endif
