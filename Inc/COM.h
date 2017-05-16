#ifndef __COMM_H__
#define __COMM_H__
#ifdef __cplusplus
extern "C"{
#endif

typedef enum
{
	COM1,
	COM2,
	COM3,
	COM4,
	COM5,
	COM_TOTAL
}COM_Number;
#ifdef __STM32F1xx_HAL_H
#include "Queue.h"
#include "BufferQueue.h"
#include "IO.h"
#define COM_485_DIR_RX		GPIO_PIN_RESET
#define COM_485_DIR_TX		GPIO_PIN_SET

#define COM_DMA_RX_SIZE		248

#define COM1_USART			USART1
#define COM1_BAUDRATE		115200
#define COM1_IRQn			USART1_IRQn
#define COM1_IRQHandler		USART1_IRQHandler
#define COM1_DmaTx_IRQF		USART1_DMA_TX_IRQF
#define COM1_DmaRx_IRQF		USART1_DMA_RX_IRQF

#define COM2_USART			USART2
#define COM2_BAUDRATE		115200
#define COM2_IRQn			USART2_IRQn
#define COM2_IRQHandler		USART2_IRQHandler
#define COM2_DmaTx_IRQF		USART2_DMA_TX_IRQF
#define COM2_DmaRx_IRQF		USART2_DMA_RX_IRQF

#define COM3_USART			USART3
#define COM3_BAUDRATE		115200
#define COM3_IRQn			USART3_IRQn
#define COM3_IRQHandler		USART3_IRQHandler
#define COM3_DmaTx_IRQF		USART3_DMA_TX_IRQF
#define COM3_DmaRx_IRQF		USART3_DMA_RX_IRQF

#define COM4_USART			UART4
#define COM4_BAUDRATE		115200
#define COM4_IRQn			UART4_IRQn
#define COM4_IRQHandler		UART4_IRQHandler
#define COM4_DmaTx_IRQF		USART4_DMA_TX_IRQF
#define COM4_DmaRx_IRQF		USART4_DMA_RX_IRQF

#define COM5_USART			UART5
#define COM5_BAUDRATE		115200
#define COM5_IRQn			UART5_IRQn
#define COM5_IRQHandler		UART5_IRQHandler
#define COM5_DmaTx_IRQF		USART5_DMA_TX_IRQF
#define COM5_DmaRx_IRQF		USART5_DMA_RX_IRQF

#define USART1_USE_REMAP	0
#if USART1_USE_REMAP
#define USART1_TX_PIN		PinCode(PORTB, GPIO_PIN_6)
#define USART1_RX_PIN		PinCode(PORTB, GPIO_PIN_7)
#else
#define USART1_TX_PIN		PinCode(PORTA, GPIO_PIN_9)
#define USART1_RX_PIN		PinCode(PORTA, GPIO_PIN_10)
#endif
#define USART1_USE_485		0
#define USART1_485_DIR_PIN	PinCode(PORTA, GPIO_PIN_0)
#define USART1_USE_DMA		0
#define USART1_DMA_TX_CH	DMA1_Channel4
#define USART1_DMA_TX_IRQ	DMA1_Channel4_IRQn
#define USART1_DMA_TX_IRQF	DMA1_Channel4_IRQHandler
#define USART1_DMA_RX_CH	DMA1_Channel5
#define USART1_DMA_RX_IRQ	DMA1_Channel5_IRQn
#define USART1_DMA_RX_IRQF	DMA1_Channel5_IRQHandler

#define USART2_USE_REMAP	1
#if USART2_USE_REMAP
#define USART2_TX_PIN		PinCode(PORTD, GPIO_PIN_5)
#define USART2_RX_PIN		PinCode(PORTD, GPIO_PIN_6)
#else
#define USART2_TX_PIN		PinCode(PORTA, GPIO_PIN_2)
#define USART2_RX_PIN		PinCode(PORTA, GPIO_PIN_3)
#endif
#define USART2_USE_485		0
#define USART2_485_DIR_PIN	PinCode(PORTA, GPIO_PIN_0)
#define USART2_USE_DMA		1
#define USART2_DMA_TX_CH	DMA1_Channel7
#define USART2_DMA_TX_IRQ	DMA1_Channel7_IRQn
#define USART2_DMA_TX_IRQF	DMA1_Channel7_IRQHandler
#define USART2_DMA_RX_CH	DMA1_Channel6
#define USART2_DMA_RX_IRQ	DMA1_Channel6_IRQn
#define USART2_DMA_RX_IRQF	DMA1_Channel6_IRQHandler

#define USART3_USE_REMAP	0
#if USART3_USE_REMAP ==		2
#define USART3_TX_PIN		PinCode(PORTD, GPIO_PIN_8)
#define USART3_RX_PIN		PinCode(PORTD, GPIO_PIN_9)
#elif USART3_USE_REMAP ==	1
#define USART3_TX_PIN		PinCode(PORTC, GPIO_PIN_10)
#define USART3_RX_PIN		PinCode(PORTC, GPIO_PIN_11)
#else
#define USART3_TX_PIN		PinCode(PORTB, GPIO_PIN_10)
#define USART3_RX_PIN		PinCode(PORTB, GPIO_PIN_11)
#endif
#define USART3_USE_485		0
#define USART3_485_DIR_PIN	PinCode(PORTA, GPIO_PIN_0)
#define USART3_USE_DMA		0
#define USART3_DMA_TX_CH	DMA1_Channel2
#define USART3_DMA_TX_IRQ	DMA1_Channel2_IRQn
#define USART3_DMA_TX_IRQF	DMA1_Channel2_IRQHandler
#define USART3_DMA_RX_CH	DMA1_Channel3
#define USART3_DMA_RX_IRQ	DMA1_Channel3_IRQn
#define USART3_DMA_RX_IRQF	DMA1_Channel3_IRQHandler

#define USART4_USE_REMAP	0
#if USART4_USE_REMAP
#error USART4不能REMAP
#else
#define USART4_TX_PIN		PinCode(PORTC, GPIO_PIN_10)
#define USART4_RX_PIN		PinCode(PORTC, GPIO_PIN_11)
#endif
#define USART4_USE_485		0
#define USART4_485_DIR_PIN	PinCode(PORTA, GPIO_PIN_0)
#define USART4_USE_DMA		0
#define USART4_DMA_TX_CH	DMA2_Channel5
#define USART4_DMA_TX_IRQ	DMA2_Channel4_5_IRQn
#define USART4_DMA_TX_IRQF	DMA2_Channel4_5_IRQHandler
#define USART4_DMA_RX_CH	DMA2_Channel3
#define USART4_DMA_RX_IRQ	DMA2_Channel3_IRQn
#define USART4_DMA_RX_IRQF	DMA2_Channel3_IRQHandler

#define USART5_USE_REMAP	0
#if USART4_USE_REMAP
#error USART5不能REMAP
#else
#define USART5_TX_PIN		PinCode(PORTC, GPIO_PIN_12)
#define USART5_RX_PIN		PinCode(PORTD, GPIO_PIN_2)
#endif
#define USART5_USE_485		0
#define USART5_485_DIR_PIN	PinCode(PORTA, GPIO_PIN_0)
#define USART5_USE_DMA		0
#if USART5_USE_DMA
#error UART5不能使用DMA
#endif
#define USART5_DMA_TX_CH	DMA2_Channel5
#define USART5_DMA_TX_IRQ	DMA2_Channel4_5_IRQn
#define USART5_DMA_TX_IRQF	DMA2_Channel4_5_IRQHandler
#define USART5_DMA_RX_CH	DMA2_Channel3
#define USART5_DMA_RX_IRQ	DMA2_Channel3_IRQn
#define USART5_DMA_RX_IRQF	DMA2_Channel3_IRQHandler

#define USART_DMA_ON_OFF	(USART1_USE_DMA | USART2_USE_DMA | USART3_USE_DMA | USART4_USE_DMA | USART5_USE_DMA)

#define COM_LIST(W)			{COM1_##W,COM2_##W,COM3_##W,COM4_##W, COM5_##W}
#define USART_LIST(W)		{USART1_##W,USART2_##W,USART3_##W,USART4_##W,USART5_##W}

typedef struct _DmaTxPara
{
	unsigned char * pData;
	unsigned int Lenth;
}DmaTxPara;


typedef struct _DmaRxPara
{
	int Size;
	int Tal;
	unsigned char Buffer[COM_DMA_RX_SIZE];
}DmaRxPara;


typedef struct _ComChannel
{
	struct
	{
		UART_HandleTypeDef Uart;
		IO_HandleTypeDef TxPin;
		IO_HandleTypeDef RxPin;
		IO_HandleTypeDef Dir485Pin;
		#if USART_DMA_ON_OFF
		DMA_HandleTypeDef DMARX;
		DMA_HandleTypeDef DMATX;
		#endif
	}Hardware;
	struct
	{
		unsigned char Remap : 2;
		unsigned char Use485: 1;
		unsigned char UseDma: 1;
		unsigned char Enable: 1;
		unsigned char Sending:1;
		unsigned char FreeFlag:1;
	}ControlBits;
	Queue * TxQueue;
	Queue * RxQueue;
	#if USART_DMA_ON_OFF
	void* SendingAddr;
	BufferQueue DmaTxQueue;
	BufferQueue DmaRxQueue;
	#endif
}ComChannel, *pComChannel;

typedef struct _ComHandle
{
	ComChannel Channel[COM_TOTAL];
}ComHandle, *pComHandle;
#endif

void COM_Initilize(COM_Number ComId);

void COM_SetEnable(COM_Number ComId, unsigned char Enable);
unsigned char COM_GetEnable(COM_Number ComId);
void COM_SetBaudrate(COM_Number ComId, unsigned int Baudrate);
void COM_SetFreeFlag(COM_Number ComId, unsigned char FreeFlag);

void COM_Transmit(COM_Number ComId, unsigned char Data);
void COM_Transmits(COM_Number ComId, unsigned char *pData, unsigned int num);
void COM_TransmitStr(COM_Number ComId, char*Data);
unsigned int COM_GetRecivedDataSize(COM_Number ComId);
unsigned char COM_GetRecivedData(COM_Number ComId);
unsigned char COM_GetRecivedTal(COM_Number ComId);
unsigned int COM_GetRecivedDatas(COM_Number ComId, unsigned char *pData, unsigned int NumberToRead);
void COM_ClearReciveBuffer(COM_Number ComId);
void COM_ClearTransferBuffer(COM_Number ComId);


typedef struct _COM_Driver
{
	void (*Initilize)(void);
	void (*SetEnable)(unsigned char Enable);
	unsigned char (*GetEnable)(void);
	void (*SetFreeFlag)(unsigned char FreeFlag);
	void (*SetBaudrate)(unsigned int Baudrate);

	void (*Transmit)(unsigned char Data);
	void (*Transmits)(unsigned char *pData, unsigned int num);
	void (*TransmitStr)(char*Data);
	unsigned int (*GetRecivedDataSize)(void);
	unsigned char (*GetRecivedData)(void);
	unsigned char (*GetRecivedTal)(void);
	unsigned int (*GetRecivedDatas)(unsigned char *pData, unsigned int NumberToRead);
	void (*ClearReciveBuffer)(void);
	void (*ClearTransferBuffer)(void);
}COM_Driver, *pCOM_Driver;

extern COM_Driver COM1Driver, COM2Driver, COM3Driver, COM4Driver, COM5Driver;
extern pCOM_Driver COMS[];
#define COM_DRIVER_DEF(A)	\
	void COM##A##_Initilize(){COM_Initilize(COM##A##);}\
	void COM##A##_SetEnable(unsigned char Enable){COM_SetEnable(COM##A##, Enable);}\
	unsigned char COM##A##_GetEnable(){return COM_GetEnable(COM##A##);}\
	void COM##A##_SetFreeFlag(unsigned char FreeFlag){COM_SetFreeFlag(COM##A##, FreeFlag);}\
	void COM##A##_SetBaudrate(unsigned int Baudrate){COM_SetBaudrate(COM##A##, Baudrate);}\
	void COM##A##_Transmit(unsigned char Data){COM_Transmit(COM##A##, Data);}\
	void COM##A##_Transmits(unsigned char *pData, unsigned int num){COM_Transmits(COM##A##, pData, num);}\
	void COM##A##_TransmitStr(char*Data){COM_TransmitStr(COM##A##, Data);}\
	unsigned int COM##A##_GetRecivedDataSize(){return COM_GetRecivedDataSize(COM##A##);}\
	unsigned char COM##A##_GetRecivedData(){return COM_GetRecivedData(COM##A##);}\
	unsigned char COM##A##_GetRecivedTal(){return COM_GetRecivedTal(COM##A##);}\
	unsigned int COM##A##_GetRecivedDatas(unsigned char *pData, unsigned int NumberToRead){return COM_GetRecivedDatas(COM##A##, pData, NumberToRead);}\
	void COM##A##_ClearReciveBuffer(){COM_ClearReciveBuffer(COM##A##);}\
	void COM##A##_ClearTransferBuffer(){COM_ClearTransferBuffer(COM##A##);}\
	COM_Driver COM##A##Driver = { \
		COM##A##_Initilize,\
		COM##A##_SetEnable,\
		COM##A##_GetEnable,\
		COM##A##_SetFreeFlag,\
		COM##A##_SetBaudrate,\
		COM##A##_Transmit,\
		COM##A##_Transmits,\
		COM##A##_TransmitStr,\
		COM##A##_GetRecivedDataSize,\
		COM##A##_GetRecivedData,\
		COM##A##_GetRecivedTal,\
		COM##A##_GetRecivedDatas,\
		COM##A##_ClearReciveBuffer,\
		COM##A##_ClearTransferBuffer,\
	};

#ifdef __cplusplus
}
#endif

#endif
