/*****************************************************************************
Copyright:  进化者
File name: COM.c
Description: 提供USART的中间层驱动程序，使用将上层COM口与底层USART口隔离
Author: wangchao
Version: V0.1
Date: 2017年2月15日
History: 
2017年3月21日	王超	添加串口关闭后将缓存中数据发送完成的机制。
*****************************************************************************/
#include "stm32f1xx_hal.h"
#include "COM.h"
#include "IO.h"
#include "TinyOs.h"
#include <stdlib.h>
#include <string.h>

static ComHandle hCom = {0};
static IRQn_Type const IRQ[] = COM_LIST(IRQn);
#if USART_DMA_ON_OFF
static DMA_Channel_TypeDef * const DmaTxCh[] = USART_LIST(DMA_TX_CH);
static DMA_Channel_TypeDef * const DmaRxCh[] = USART_LIST(DMA_RX_CH);
#endif
/****************************************************
	函数名:	COM_Initilize
	功能:	初始化COM端口与其硬件资源
	参数:	COMx: 端口编号 x=1、2、3、4、5
	作者:	wangchao 2017年2月15日09:40:08
****************************************************/
void COM_Initilize(COM_Number COMx)
{
	USART_TypeDef * const ComUsart[] = COM_LIST(USART);
	const unsigned int ComBaudrate[] = COM_LIST(BAUDRATE);
	pComChannel pCom = &hCom.Channel[COMx];
	HAL_RCC_GetPCLK1Freq();
	pCom->Hardware.Uart.Instance = ComUsart[COMx];
	pCom->Hardware.Uart.Init.BaudRate = ComBaudrate[COMx];
	pCom->Hardware.Uart.Init.WordLength = UART_WORDLENGTH_8B;
	pCom->Hardware.Uart.Init.StopBits   = UART_STOPBITS_1;
	pCom->Hardware.Uart.Init.Parity     = UART_PARITY_NONE;
	pCom->Hardware.Uart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	pCom->Hardware.Uart.Init.Mode       = UART_MODE_TX_RX;
	
	HAL_NVIC_SetPriority(IRQ[COMx], 5, 0);
    HAL_NVIC_EnableIRQ(IRQ[COMx]);
	
	HAL_UART_Init(&pCom->Hardware.Uart);
	#if USART_DMA_ON_OFF
	if(pCom->ControlBits.UseDma)
	{
		void com_dmaRecive(COM_Number COMx);
		__HAL_UART_ENABLE_IT(&pCom->Hardware.Uart, UART_IT_IDLE);
		com_dmaRecive(COMx);
	}
	else
	#endif
	{
		__HAL_UART_ENABLE_IT(&pCom->Hardware.Uart, UART_IT_RXNE);
	}
	COM_SetEnable(COMx, 1);
	
}
static void com_portClock (USART_TypeDef *USARTX, FunctionalState NewState)
{

	if(NewState == ENABLE)
	{
		if      (USARTX == USART1) __HAL_RCC_USART1_CLK_ENABLE();
		else if (USARTX == USART2) __HAL_RCC_USART2_CLK_ENABLE();
		else if (USARTX == USART3) __HAL_RCC_USART3_CLK_ENABLE();
		else if (USARTX == UART4) __HAL_RCC_UART4_CLK_ENABLE();
		else if (USARTX == UART5) __HAL_RCC_UART5_CLK_ENABLE();
	}
	else
	{
		if      (USARTX == USART1) __HAL_RCC_USART1_CLK_DISABLE();
		else if (USARTX == USART2) __HAL_RCC_USART2_CLK_DISABLE();
		else if (USARTX == USART3) __HAL_RCC_USART3_CLK_DISABLE();
		else if (USARTX == UART4) __HAL_RCC_UART4_CLK_DISABLE();
		else if (USARTX == UART5) __HAL_RCC_UART5_CLK_DISABLE();
	}
}
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	
	USART_TypeDef * const USARTX[]    = {USART1, USART2, USART3, UART4, UART5};
	const unsigned long UsartTxPin[]   = USART_LIST(TX_PIN);
	const unsigned long UsartRxPin[]   = USART_LIST(RX_PIN);
	const unsigned long Usart485Pin[]  = USART_LIST(485_DIR_PIN);
	const unsigned char Remap[] = USART_LIST(USE_REMAP);
	const unsigned char Use485[] = USART_LIST(USE_485);
	const unsigned char UseDma[] = USART_LIST(USE_DMA);
	#if USART_DMA_ON_OFF
	IRQn_Type const DmaTxIRQ[] = USART_LIST(DMA_TX_IRQ);
	IRQn_Type const DmaRxIRQ[] = USART_LIST(DMA_RX_IRQ);
	#endif
	
	pComChannel pCom;
	
	int i;
	for(i = 0; i < 8; i++)
	{
		if(USARTX[i] == huart->Instance)
			break;
	}
	/*for(pCom = hCom.Channel; pCom < &hCom.Channel[COM_TOTAL]; pCom++)
	{
		if(&pCom->Hardware.Uart == huart)
			break;
	}*/
	pCom = (pComChannel)huart;
	
	
	
	com_portClock(huart->Instance, ENABLE);
	
	pCom->ControlBits.Remap = Remap[i];
	pCom->ControlBits.Use485 = Use485[i];
	pCom->ControlBits.UseDma = UseDma[i];
	
	pCom->Hardware.TxPin.PinId = UsartTxPin[i];
	pCom->Hardware.TxPin.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	pCom->Hardware.TxPin.Init.Mode = GPIO_MODE_AF_PP;
	IO_Init(&pCom->Hardware.TxPin);
	
	pCom->Hardware.RxPin.PinId = UsartRxPin[i];
	pCom->Hardware.RxPin.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	pCom->Hardware.RxPin.Init.Mode = GPIO_MODE_INPUT;
	pCom->Hardware.RxPin.Init.Pull = GPIO_NOPULL;
	IO_Init(&pCom->Hardware.RxPin);
	
	if(pCom->ControlBits.Remap)
	{
		__HAL_RCC_AFIO_CLK_ENABLE();
		switch(i)
		{
			case 0:
				__HAL_AFIO_REMAP_USART1_ENABLE();
				break;
			case 1:
				__HAL_AFIO_REMAP_USART2_ENABLE();
				break;
			case 2:
				if(pCom->ControlBits.Remap == 2)
					__HAL_AFIO_REMAP_USART3_ENABLE();
				else
					__HAL_AFIO_REMAP_USART3_PARTIAL();
				break;
			default:
				break;
		}
	}
	
	if(pCom->ControlBits.Use485)
	{
		pCom->Hardware.Dir485Pin.PinId = Usart485Pin[i];
		pCom->Hardware.Dir485Pin.Init.Speed = GPIO_SPEED_FREQ_HIGH;
		pCom->Hardware.Dir485Pin.Init.Mode = GPIO_MODE_OUTPUT_PP;
		IO_Init(&pCom->Hardware.Dir485Pin);
		IO_WritePin(&pCom->Hardware.Dir485Pin, COM_485_DIR_RX);
	}
	#if USART_DMA_ON_OFF
	if(pCom->ControlBits.UseDma)
	{
		
		if(i < 3)
		{
			__HAL_RCC_DMA1_CLK_ENABLE();
		}
		else
		{
			__HAL_RCC_DMA2_CLK_ENABLE();
		}
		pCom->Hardware.DMARX.Instance = DmaRxCh[i];
		pCom->Hardware.DMARX.Init.Direction = DMA_PERIPH_TO_MEMORY;
		pCom->Hardware.DMARX.Init.PeriphInc = DMA_PINC_DISABLE;
		pCom->Hardware.DMARX.Init.MemInc = DMA_MINC_ENABLE;
		pCom->Hardware.DMARX.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		pCom->Hardware.DMARX.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		pCom->Hardware.DMARX.Init.Mode = DMA_CIRCULAR;
		pCom->Hardware.DMARX.Init.Priority = DMA_PRIORITY_LOW;
		HAL_DMA_Init(&pCom->Hardware.DMARX);

		__HAL_LINKDMA(&pCom->Hardware.Uart,hdmarx,pCom->Hardware.DMARX);
		
		pCom->Hardware.DMATX.Instance = DmaTxCh[i];
		pCom->Hardware.DMATX.Init.Direction = DMA_MEMORY_TO_PERIPH;
		pCom->Hardware.DMATX.Init.PeriphInc = DMA_PINC_DISABLE;
		pCom->Hardware.DMATX.Init.MemInc = DMA_MINC_ENABLE;
		pCom->Hardware.DMATX.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		pCom->Hardware.DMATX.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		pCom->Hardware.DMATX.Init.Mode = DMA_NORMAL;
		pCom->Hardware.DMATX.Init.Priority = DMA_PRIORITY_LOW;
		HAL_DMA_Init(&pCom->Hardware.DMATX);
		__HAL_LINKDMA(&pCom->Hardware.Uart,hdmatx,pCom->Hardware.DMATX);
		
		BQ_Init(&pCom->DmaTxQueue, sizeof(DmaTxPara));
		HAL_NVIC_SetPriority(DmaTxIRQ[i], 1, 0);
		HAL_NVIC_EnableIRQ(DmaTxIRQ[i]);
		
		BQ_Init(&pCom->DmaRxQueue, sizeof(DmaRxPara));
		HAL_NVIC_SetPriority(DmaRxIRQ[i], 0, 0);
		HAL_NVIC_EnableIRQ(DmaRxIRQ[i]);
	}
	else
	#endif
	{
		pCom->TxQueue = malloc(sizeof(Queue));
		Queue_Initilize(pCom->TxQueue);
		pCom->RxQueue = malloc(sizeof(Queue));
		Queue_Initilize(pCom->RxQueue);
	}
}
/****************************************************
	函数名:	COM_SetEnable
	功能:	打开或关闭串口
	参数:	COMx: 串口编号；Enable: 串口使能标识
	作者:	wangchao 2017年2月15日09:40:37
****************************************************/
void COM_SetEnable(COM_Number COMx, unsigned char Enable)
{
	pComChannel pCom = &hCom.Channel[COMx];
	pCom->ControlBits.Enable = Enable;
	if(Enable)
	{
		HAL_NVIC_EnableIRQ(IRQ[COMx]);
	}
	else
	{
		if(!pCom->ControlBits.Sending)
			HAL_NVIC_DisableIRQ(IRQ[COMx]);
	}
}
void COM_SetFreeFlag(COM_Number COMx, unsigned char FreeFlag)
{
	hCom.Channel[COMx].ControlBits.FreeFlag = FreeFlag;
}
/****************************************************
	函数名:	COM_GetEnable
	功能:	获得COM口的打开，关闭状态
	参数:	COMx: 串口编号
	作者:	wangchao 2017年2月15日09:42:53
****************************************************/
unsigned char COM_GetEnable(COM_Number COMx)
{
	return hCom.Channel[COMx].ControlBits.Enable;
}
/****************************************************
	函数名:	COM_SetBaudrate
	功能:	设置串口波特率
	参数:	COMx: 串口编号；Baudrate: 波特率
	作者:	wangchao 2017年2月15日09:44:02
****************************************************/
void COM_SetBaudrate(COM_Number COMx, unsigned int Baudrate)
{
	unsigned char enable = COM_GetEnable(COMx);
	
	if(enable)
	{
		COM_SetEnable(COMx, 0);
	}
	hCom.Channel[COMx].Hardware.Uart.Init.BaudRate = Baudrate;
	HAL_UART_Init(&hCom.Channel[COMx].Hardware.Uart);
	if(enable)
	{
		COM_SetEnable(COMx, 1);
	}
}
/****************************************************
	函数名:	COM_Transmit
	功能:	COM口发送一个字节
	参数:	COMx: 串口编号；Data: 要发送的字节
	作者:	wangchao 2017年2月15日09:45:16
****************************************************/
void COM_Transmit(COM_Number COMx, unsigned char Data)
{
	pComChannel pCom = &hCom.Channel[COMx];
	pCom->ControlBits.Sending = 1;
	if(pCom->ControlBits.UseDma)
	{
		COM_Transmits(COMx, &Data, 1);
	}
	else
	{
		if(!Queue_Full(pCom->TxQueue))
		{
			if(pCom->ControlBits.Use485)
			{
				IO_WritePin(&pCom->Hardware.Dir485Pin, COM_485_DIR_TX);
			}
			Queue_Push(pCom->TxQueue, Data);
			__HAL_UART_ENABLE_IT(&pCom->Hardware.Uart, UART_IT_TXE);
		}
	}
}
/****************************************************
	函数名:	COM_Transmits
	功能:	COM口发送多个字节
	参数:	COMx: 串口编号；pData: 要发送数据起始地址；num: 要发送的数据长度
	作者:	wangchao 2017年2月15日09:46:57
****************************************************/
void COM_Transmits(COM_Number COMx, unsigned char *pData, unsigned int num)
{
	unsigned int i;
	hCom.Channel[COMx].ControlBits.Sending = 1;
	#if USART_DMA_ON_OFF
	if(hCom.Channel[COMx].ControlBits.UseDma)
	{
		void com_dmaTransfer(COM_Number COMx);
		DmaTxPara * TxPara = (DmaTxPara *)BQ_NewBuffer(&hCom.Channel[COMx].DmaTxQueue);
		TxPara -> pData = pData;
		TxPara -> Lenth = num;
		if(hCom.Channel[COMx].Hardware.Uart.State == HAL_UART_STATE_BUSY_RX || hCom.Channel[COMx].Hardware.Uart.State == HAL_UART_STATE_READY)
		{
			if(hCom.Channel[COMx].ControlBits.Use485)
			{
				IO_WritePin(&hCom.Channel[COMx].Hardware.Dir485Pin, COM_485_DIR_TX);
			}
			com_dmaTransfer(COMx);
		}
		
	}
	else
	#endif
	{
		for(i=0; i<num; i++)
		{
			Queue_Push(hCom.Channel[COMx].TxQueue, pData[i]);
		}
		__HAL_UART_ENABLE_IT(&hCom.Channel[COMx].Hardware.Uart, UART_IT_TXE);
	}
}
/****************************************************
	函数名:	COM_TransmitStr
	功能:	COM口发送一个字符串，不发送'\0'
	参数:	COMx: 串口编号；pData: 字符串首地址
	作者:	wangchao 2017年2月15日09:49:37
****************************************************/
void COM_TransmitStr(COM_Number COMx, char *pData)
{
	COM_Transmits(COMx, (unsigned char*)pData, strlen(pData));
}
/****************************************************
	函数名:	COM_ClearReciveBuffer
	功能:	清除COM端口的接收缓存
	参数:	COMx: 串口编号
	作者:	wangchao 2017年2月15日09:57:43
****************************************************/
void COM_ClearReciveBuffer(COM_Number COMx)
{
	pComChannel ch = &hCom.Channel[COMx];

	#if USART_DMA_ON_OFF
	if(ch->ControlBits.UseDma)
	{
	}
	else
	#endif
	{
		Queue_Initilize(ch->RxQueue);
	}
}
/****************************************************
	函数名:	COM_ClearTransferBuffer
	功能:	清除COM端口的发送缓存
	参数:	COMx: 串口编号
	作者:	wangchao 2017年2月15日09:57:43
****************************************************/
void COM_ClearTransferBuffer(COM_Number COMx)
{
	pComChannel ch = &hCom.Channel[COMx];
	#if USART_DMA_ON_OFF
	if(ch->ControlBits.UseDma)
	{
	}
	else
	#endif
	Queue_Initilize(ch->TxQueue);
}
/****************************************************
	函数名:	COM_GetRecivedDataSize
	功能:	获取串口接收缓存中未接受的字符长度
	参数:	COMx: 串口编号
	返回:	串口接收缓存中未接受的字符长度
	作者:	wangchao 2017年2月15日10:06:24
****************************************************/
unsigned int COM_GetRecivedDataSize(COM_Number COMx)
{
	pComChannel ch = &hCom.Channel[COMx];
	
	#if USART_DMA_ON_OFF
	pListNode p;
	if(ch->ControlBits.UseDma)
	{
		unsigned int Sum = 0;
		for(p = ch->DmaRxQueue.list.Head; NULL_NODE != p; p = p->Next)
		{
			DmaRxPara * rx = (DmaRxPara*)p->Buffer;
			if(rx)
				Sum += rx->Size - rx->Tal;
		}
		return Sum;
	}
	#endif
	return Queue_Size(ch->RxQueue);
}
/****************************************************
	函数名:	COM_GetRecivedData
	功能:	读取串口接收缓存中未接受的一个字符
	参数:	COMx: 串口编号
	返回:	接收队列尾部一个字节
	作者:	wangchao 2017年2月15日10:09:16
****************************************************/
unsigned char COM_GetRecivedData(COM_Number COMx)
{
	pComChannel ch = &hCom.Channel[COMx];
	unsigned char Data = 0;
	#if USART_DMA_ON_OFF
	if(ch->ControlBits.UseDma)
	{
		COM_GetRecivedDatas(COMx, &Data, 1);
	}
	else
	#endif
	if(!Queue_Empty(ch->RxQueue))
	{
		Data = Queue_Pop(ch->RxQueue);
	}
	return Data;
}
/****************************************************
	函数名:	COM_GetRecivedTal
	功能:	读取串口接收缓存中未接受的一个字符, 不改变队列状态
	参数:	COMx: 串口编号
	返回:	接收队列尾部一个字节
	作者:	wangchao 2017年2月15日10:09:16
****************************************************/
unsigned char COM_GetRecivedTal(COM_Number COMx)
{
	pComChannel ch = &hCom.Channel[COMx];
	#if USART_DMA_ON_OFF
	if(ch->ControlBits.UseDma)
	{
		DmaRxPara * rx = (DmaRxPara *)BQ_GetPop(&ch->DmaRxQueue);
		if(rx->Size > rx->Tal)
			return rx->Buffer[rx->Tal];
		return 0;
	}
	#endif
	return Queue_GetPop(ch->RxQueue);
}
/****************************************************
	函数名:	COM_GetRecivedData
	功能:	读取串口接收缓存中未接受的多个字符
	参数:	COMx: 串口编号；pData: 读取结果保存缓存首地址；NumberToRead: 准备读取的长度，若接收缓存长度小于此数，则全读完
	返回:	实际读取个数
	作者:	wangchao 2017年2月15日10:09:16
****************************************************/
unsigned int COM_GetRecivedDatas(COM_Number COMx, unsigned char *pData, unsigned int NumberToRead)
{
	unsigned int i;
	pComChannel ch = &hCom.Channel[COMx];
	#if USART_DMA_ON_OFF
	if(ch->ControlBits.UseDma)
	{
		DmaRxPara * rx = (DmaRxPara *)BQ_GetPop(&ch->DmaRxQueue);
		unsigned char *p = pData;
		while(NumberToRead > 0 && BQ_Size(&ch->DmaRxQueue) > 1)
		{
			if(NumberToRead >= rx->Size - rx->Tal)
			{
				memcpy(p, &rx->Buffer[rx->Tal], rx->Size - rx->Tal);
				NumberToRead -= rx->Size - rx->Tal;
				p += rx->Size - rx->Tal;
				BQ_Pop(&ch->DmaRxQueue);
				rx = (DmaRxPara *)BQ_GetPop(&ch->DmaRxQueue);
			}
			else
			{
				memcpy(p, &rx->Buffer[rx->Tal], NumberToRead);
				p += NumberToRead;
				rx->Tal += NumberToRead;
				NumberToRead = 0;
			}
		}
		return p - pData;
	}
	#endif
	i = Queue_Size(ch->RxQueue);
	if(NumberToRead > i)
	{
		NumberToRead = i;
	}
	
	for(i=0; i<NumberToRead; i++)
	{
		pData[i] = Queue_Pop(ch->RxQueue);
	}
	return NumberToRead;
}


static void COM_IRQHandler(COM_Number ComId)
{
	unsigned char Data;
	pComChannel ch = &hCom.Channel[ComId];
	#ifdef __TINYOS_H__
	unsigned int ComRx = 0;
	OS_EnterInterruptMode();
	#endif
	#if USART_DMA_ON_OFF
	if(ch->ControlBits.UseDma)
	{
		void com_dmaRecive(COM_Number COMx);
		if(__HAL_UART_GET_FLAG(&ch->Hardware.Uart, UART_FLAG_IDLE))
		{
			DmaRxPara *rx;
			__HAL_UART_CLEAR_FLAG(&ch->Hardware.Uart, UART_FLAG_IDLE);
			(void)ch->Hardware.Uart.Instance->DR;
			HAL_DMA_Abort(ch->Hardware.Uart.hdmarx);
			rx = (DmaRxPara *)BQ_GetHead(&ch->DmaRxQueue);
			rx->Size = COM_DMA_RX_SIZE - ch->Hardware.Uart.hdmarx->Instance->CNDTR;
			if(ch->Hardware.Uart.State == HAL_UART_STATE_BUSY_TX_RX)
			{
			  ch->Hardware.Uart.State = HAL_UART_STATE_BUSY_TX;
			}
			else
			{
			  ch->Hardware.Uart.State = HAL_UART_STATE_READY;
			}
			com_dmaRecive(ComId);
			#ifdef __TINYOS_H__
			ComRx = 1;
			#endif
		}
		if(__HAL_UART_GET_FLAG(&ch->Hardware.Uart, UART_FLAG_TXE))
		{
			__HAL_UART_CLEAR_FLAG(&ch->Hardware.Uart, UART_FLAG_TC);
			if(ch->ControlBits.Use485)
			{
				IO_WritePin(&ch->Hardware.Dir485Pin, COM_485_DIR_RX);
			}
			ch->ControlBits.Sending = 0;
			if(!ch->ControlBits.Enable)
				HAL_NVIC_DisableIRQ(IRQ[ComId]);
		}
	}
	else
	#endif
	{
		if(__HAL_UART_GET_FLAG(&ch->Hardware.Uart, UART_FLAG_RXNE))
		{
			__HAL_UART_CLEAR_FLAG(&ch->Hardware.Uart, UART_FLAG_RXNE);
			Data = ch->Hardware.Uart.Instance->DR;
			Queue_Push(ch->RxQueue, Data);
			#ifdef __TINYOS_H__
			ComRx = 1;
			#endif
		}
		if(__HAL_UART_GET_FLAG(&ch->Hardware.Uart, UART_FLAG_TXE))
		{
			__HAL_UART_CLEAR_FLAG(&ch->Hardware.Uart, UART_FLAG_TC);
			if(Queue_Empty(ch->TxQueue))
			{
				__HAL_UART_DISABLE_IT(&ch->Hardware.Uart, UART_IT_TXE);
				if(ch->ControlBits.Use485)
				{
					IO_WritePin(&hCom.Channel[ComId].Hardware.Dir485Pin, COM_485_DIR_RX);
				}
				ch->ControlBits.Sending = 0;
				if(!ch->ControlBits.Enable)
				HAL_NVIC_DisableIRQ(IRQ[ComId]);
			}
			else
			{
				Data = Queue_Pop(ch->TxQueue);
				ch->Hardware.Uart.Instance->DR = Data;
			}
		}
	}
	__HAL_UART_CLEAR_FLAG(&ch->Hardware.Uart, UART_FLAG_TC | UART_FLAG_IDLE | UART_FLAG_RXNE);
	#ifdef __TINYOS_H__
	OS_ExitInterruptMode();
	if(ComRx)
		Task_Trig((char*)"COM", (void*)0, 0);
	#endif
}
void COM1_IRQHandler(){COM_IRQHandler(COM1);}
void COM2_IRQHandler(){COM_IRQHandler(COM2);}
void COM3_IRQHandler(){COM_IRQHandler(COM3);}
void COM4_IRQHandler(){COM_IRQHandler(COM4);}
void COM5_IRQHandler(){COM_IRQHandler(COM5);}

#if USART_DMA_ON_OFF
static void com_dmaTransfer(COM_Number COMx)
{
	pComChannel pCom = &hCom.Channel[COMx];
	
	DmaTxPara *txPara = (DmaTxPara *)BQ_GetPop(&pCom->DmaTxQueue);
	pCom->SendingAddr = txPara->pData;
	HAL_UART_Transmit_DMA(&pCom->Hardware.Uart, txPara->pData, txPara->Lenth);
	BQ_Pop(&pCom->DmaTxQueue);
}
static void com_dmaRecive(COM_Number COMx)
{
	pComChannel pCom = &hCom.Channel[COMx];
	DmaRxPara *rx;
	rx = (DmaRxPara *)BQ_NewBuffer(&pCom->DmaRxQueue);
	
	rx->Tal = rx->Size = 0;
	HAL_UART_Receive_DMA(&pCom->Hardware.Uart,rx->Buffer, COM_DMA_RX_SIZE);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	COM_Number ComId;
	#ifdef __TINYOS_H__
	OS_EnterInterruptMode();
	#endif
	for(ComId = COM1; ComId < COM_TOTAL; ComId++)
	{
		if(huart == &hCom.Channel[ComId].Hardware.Uart)
		{
			break;
		}
	}
	if(huart->State == HAL_UART_STATE_BUSY_TX_RX)
    {
      huart->State = HAL_UART_STATE_BUSY_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_READY;
    }
	if(hCom.Channel[ComId].ControlBits.FreeFlag)
	{
		free(hCom.Channel[ComId].SendingAddr);
	}
	if(!BQ_Empty(&hCom.Channel[ComId].DmaTxQueue))
	{
		com_dmaTransfer(ComId);
	}
	#ifdef __TINYOS_H__
	OS_ExitInterruptMode();
	#endif
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	COM_Number ComId;
	DmaRxPara * a;
	#ifdef __TINYOS_H__
	OS_EnterInterruptMode();
	#endif
	HAL_DMA_Abort(huart->hdmarx);
	for(ComId = COM1; ComId < COM_TOTAL; ComId++)
	{
		if(huart == &hCom.Channel[ComId].Hardware.Uart)
		{
			break;
		}
	}
	if(hCom.Channel[ComId].Hardware.Uart.State == HAL_UART_STATE_BUSY_TX_RX)
	{
		hCom.Channel[ComId].Hardware.Uart.State = HAL_UART_STATE_BUSY_TX;
	}
	else
	{
		hCom.Channel[ComId].Hardware.Uart.State = HAL_UART_STATE_READY;
	}
	a = (DmaRxPara *)BQ_GetHead(&hCom.Channel[ComId].DmaRxQueue);
	a->Size = COM_DMA_RX_SIZE;
	
	com_dmaRecive(ComId);
	#ifdef __TINYOS_H__
	OS_ExitInterruptMode();
	Task_Trig((char*)"COM", (void*)0, 0);
	#endif
}
void COM1_DmaTx_IRQF(void){HAL_DMA_IRQHandler(&hCom.Channel[COM1].Hardware.DMATX);}
void COM2_DmaTx_IRQF(void){HAL_DMA_IRQHandler(&hCom.Channel[COM2].Hardware.DMATX);}
void COM3_DmaTx_IRQF(void){HAL_DMA_IRQHandler(&hCom.Channel[COM3].Hardware.DMATX);}
void COM4_DmaTx_IRQF(void){HAL_DMA_IRQHandler(&hCom.Channel[COM4].Hardware.DMATX);}
void COM1_DmaRx_IRQF(void){HAL_DMA_IRQHandler(&hCom.Channel[COM1].Hardware.DMARX);}
void COM2_DmaRx_IRQF(void){HAL_DMA_IRQHandler(&hCom.Channel[COM2].Hardware.DMARX);}
void COM3_DmaRx_IRQF(void){HAL_DMA_IRQHandler(&hCom.Channel[COM3].Hardware.DMARX);}
void COM4_DmaRx_IRQF(void){HAL_DMA_IRQHandler(&hCom.Channel[COM4].Hardware.DMARX);}
#endif
COM_DRIVER_DEF(1);
COM_DRIVER_DEF(2);
COM_DRIVER_DEF(3);
COM_DRIVER_DEF(4);
COM_DRIVER_DEF(5);
pCOM_Driver COMS[] = {&COM1Driver, &COM2Driver, &COM3Driver, &COM4Driver, &COM5Driver};
