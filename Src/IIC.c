#include "stm32f1xx_hal.h"
#include "IO.h"
#include "IIC.h"
IIC_DRIVER_DEF(1);
IIC_DRIVER_DEF(2);
pIIC_Driver IIC[IIC_Total] = {&IIC1Driver, &IIC2Driver};
IIC_Handle hIIC;
void IIC_Initilize(IIC_Index index)
{
	hIIC.Channel[index].hI2C.Instance = index ? I2C2 : I2C1;
	hIIC.Channel[index].hI2C.Init.ClockSpeed = 100000;
	hIIC.Channel[index].hI2C.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hIIC.Channel[index].hI2C.Init.OwnAddress1 = 0;
	hIIC.Channel[index].hI2C.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hIIC.Channel[index].hI2C.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hIIC.Channel[index].hI2C.Init.OwnAddress2 = 0;
	hIIC.Channel[index].hI2C.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hIIC.Channel[index].hI2C.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	
	HAL_I2C_Init(&hIIC.Channel[index].hI2C);
}
void IIC_DeInitilize(IIC_Index index)
{
	HAL_I2C_DeInit(&hIIC.Channel[index].hI2C);
}
unsigned char IIC_Transfer(IIC_Index index, unsigned short DevAddress, unsigned char *pData, unsigned short Size, unsigned int Timeout)
{
	return (unsigned char)HAL_I2C_Master_Transmit(&hIIC.Channel[index].hI2C, DevAddress, pData, Size, Timeout);
}
unsigned char IIC_Recive(IIC_Index index, unsigned short DevAddress, unsigned char *pData, unsigned short Size, unsigned int Timeout)
{
	return (unsigned char)HAL_I2C_Master_Receive(&hIIC.Channel[index].hI2C, DevAddress, pData, Size, Timeout);
}
unsigned char IIC_Mem_Read(IIC_Index index, unsigned short DevAddress, unsigned short MemAddress, unsigned short MemAddSize, unsigned char *pData, unsigned short Size, unsigned int Timeout)
{
	return (unsigned char)HAL_I2C_Mem_Read(&hIIC.Channel[index].hI2C, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
}
unsigned char IIC_Mem_Write(IIC_Index index, unsigned short DevAddress, unsigned short MemAddress, unsigned short MemAddSize, unsigned char *pData, unsigned short Size, unsigned int Timeout)
{
	return (unsigned char)HAL_I2C_Mem_Write(&hIIC.Channel[index].hI2C, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
}
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
	//GPIO_InitTypeDef GPIO_InitStruct;
	if(hi2c->Instance==I2C1)
	{
		__HAL_RCC_I2C1_CLK_ENABLE();
		hIIC.Channel[IIC1].SCL.PinId = IIC_SCL_PIN_CH1;
		hIIC.Channel[IIC1].SCL.Init.Speed = GPIO_SPEED_FREQ_HIGH;
		hIIC.Channel[IIC1].SCL.Init.Mode = GPIO_MODE_AF_OD;
		hIIC.Channel[IIC1].SCL.Init.Pull = GPIO_PULLUP;
		IO_Init(&hIIC.Channel[IIC1].SCL);
		hIIC.Channel[IIC1].SDA.PinId = IIC_SDA_PIN_CH1;
		hIIC.Channel[IIC1].SDA.Init.Speed = GPIO_SPEED_FREQ_HIGH;
		hIIC.Channel[IIC1].SDA.Init.Mode = GPIO_MODE_AF_OD;
		hIIC.Channel[IIC1].SDA.Init.Pull = GPIO_PULLUP;
		IO_Init(&hIIC.Channel[IIC1].SDA);
		
		HAL_Delay(2);
		I2C1->CR1 |= I2C_CCR_FS;
		I2C1->CR1 &= ~I2C_CCR_FS;
	}
	else if(hi2c->Instance==I2C2)
	{
		__HAL_RCC_I2C2_CLK_ENABLE();
		hIIC.Channel[IIC2].SCL.PinId = IIC_SCL_PIN_CH2;
		hIIC.Channel[IIC2].SCL.Init.Speed = GPIO_SPEED_FREQ_HIGH;
		hIIC.Channel[IIC2].SCL.Init.Mode = GPIO_MODE_AF_OD;
		hIIC.Channel[IIC2].SCL.Init.Pull = GPIO_PULLUP;
		IO_Init(&hIIC.Channel[IIC2].SCL);
		hIIC.Channel[IIC2].SDA.PinId = IIC_SDA_PIN_CH2;
		hIIC.Channel[IIC2].SDA.Init.Speed = GPIO_SPEED_FREQ_HIGH;
		hIIC.Channel[IIC2].SDA.Init.Mode = GPIO_MODE_AF_OD;
		hIIC.Channel[IIC2].SDA.Init.Pull = GPIO_PULLUP;
		IO_Init(&hIIC.Channel[IIC1].SDA);
		
		HAL_Delay(2);
		I2C2->CR1 |= I2C_CCR_FS;
		I2C2->CR1 &= ~I2C_CCR_FS;
	}

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{

	if(hi2c->Instance==I2C1)
	{
		__HAL_RCC_I2C1_CLK_DISABLE();
		IO_DeInit(&hIIC.Channel[IIC1].SCL);
		IO_DeInit(&hIIC.Channel[IIC1].SDA);
		HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
		HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
	}
	else if(hi2c->Instance==I2C2)
	{
		__HAL_RCC_I2C2_CLK_DISABLE();
		IO_DeInit(&hIIC.Channel[IIC2].SCL);
		IO_DeInit(&hIIC.Channel[IIC2].SDA);
		HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
		HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
	}
	HAL_Delay(10);
}
