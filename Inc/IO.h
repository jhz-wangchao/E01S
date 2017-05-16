#ifndef __IO_H__
#define __IO_H__
#ifdef __cplusplus
extern "C"{
#endif
#define PORTA	0x0
#define PORTB	0x1
#define PORTC	0x2
#define PORTD	0x3
#define PORTE	0x4
#define PORTF	0x5
#define PORTG	0x6
#define IO_PORT_SHIFT	16
#define PinCode(Port, Pin) (((Port) << (IO_PORT_SHIFT)) | ((Pin) & 0xffff))

#ifdef __STM32F1xx_HAL_H

typedef void (*ExtiIrqCallbackDelegate)(void);

typedef struct _IO_HandleTypeDef
{
	GPIO_TypeDef *Instance;
	unsigned short Pin;
	unsigned long PinId;
	GPIO_InitTypeDef Init;
	ExtiIrqCallbackDelegate ExtiIrqCallback;
}IO_HandleTypeDef, *pIO_HandleTypeDef;
typedef GPIO_TypeDef PortBaseAddr;
#else
typedef void PortBaseAddr;
typedef void *pIO_HandleTypeDef;
#endif

void GPIO_PortClock (PortBaseAddr *GPIOx, unsigned char enable);
unsigned char GPIO_GetPortClockState (PortBaseAddr *GPIOx);

void IO_Init(pIO_HandleTypeDef hIo);
void IO_DeInit(pIO_HandleTypeDef hIo);
unsigned char IO_ReadPin(pIO_HandleTypeDef hIo);
void IO_WritePin(pIO_HandleTypeDef hIo, unsigned char PinState);
void  IO_TogglePin(pIO_HandleTypeDef hIo);
unsigned char IO_ReadPinBuffer(pIO_HandleTypeDef hIo);
#ifdef __cplusplus
}
#endif
#endif
