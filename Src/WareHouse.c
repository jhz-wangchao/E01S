#include "stm32f1xx_hal.h"
#include "IO.h"
#include "ADC.h"
#include "IIC.h"
#include "WareHouse.h"
WareHouseHandle hWh={WAREHOUSE_DECETED_UNKNOWN};
unsigned char WH_Deceted(void)
{
	int v = (int)ADC_Read(ADC_WAREHOUSE_DECETED_CHANNEL);
	unsigned char dt = WAREHOUSE_DECETED_UNKNOWN;
	if(WAREHOUSE_MATCH(v, ADC_VALUE_CLEARER))
	{
		dt = WAREHOUSE_DECETED_CLEARER;
	}
	if(WAREHOUSE_MATCH(v, ADC_VALUE_DRAWER))
	{
		dt = WAREHOUSE_DECETED_DRAWER;
	}
	if(WAREHOUSE_MATCH(v, ADC_VALUE_SUBWOOFER))
	{
		dt = WAREHOUSE_DECETED_SUBWOOFER;
	}
	if(dt != hWh.DeviceType)
		WH_DeviceChange(dt);
	return dt;
}
void WH_DeviceChange(unsigned char NewDevice)
{
	if(WAREHOUSE_DECETED_DRAWER == hWh.DeviceType)
	{
		Drawer_DeInitilize();
	}
	if(WAREHOUSE_DECETED_DRAWER == NewDevice)
	{
		Drawer_Initilize();
	}
	hWh.DeviceType = NewDevice;
	WH_DeviceChangeCallback(NewDevice);
}
__weak void WH_DeviceChangeCallback(unsigned char NewDevice)
{
	(void)NewDevice;
}
void Drawer_Initilize(void)
{
	hWh.Drawer.Driver = IIC[DRAWER_IIC];
	hWh.Drawer.Driver->Initilize();
	
	hWh.Drawer.IntPin.PinId = DRAWER_INT_PIN;
	hWh.Drawer.IntPin.Init.Mode = GPIO_MODE_INPUT;
	hWh.Drawer.IntPin.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	hWh.Drawer.IntPin.Init.Pull = GPIO_PULLUP;
	IO_Init(&hWh.Drawer.IntPin);
}
void Drawer_DeInitilize(void)
{
	hWh.Drawer.Driver->DeInitilize();
	IO_DeInit(&hWh.Drawer.IntPin);
}
static unsigned char Drawer_SendData(unsigned char *Buffer, unsigned char Lenth)
{
	static unsigned char Sequence = 1;
	int i;
	unsigned sum = 0;
	
	Buffer[0] = Lenth;
	Buffer[2] = Sequence++;
	for(i=0; i<Lenth; i++)
	{
		sum += Buffer[i];
	}
	Buffer[Lenth] = sum;
	return hWh.Drawer.Driver->Tranfer(DRAWER_IIC_ADD, Buffer, Lenth + 1, 100);
}
static unsigned char Drawer_Recive(unsigned char *Buffer, unsigned char Lenth)
{
	//HAL_Delay(100);
	unsigned int tickstart = HAL_GetTick();
	extern IWDG_HandleTypeDef hIwdg;
	while(IO_ReadPin(&hWh.Drawer.IntPin))
	{
		HAL_IWDG_Refresh(&hIwdg);
		if((HAL_GetTick() - tickstart) > 1000)
		{
			return 0x01;
		}
	}
	
	return hWh.Drawer.Driver->Recive(DRAWER_IIC_ADD, Buffer, Lenth + 1, 100);
}
void Drawer_Move(unsigned char Place, unsigned char Speed)
{
	unsigned char Buffer[6];
	if(WAREHOUSE_DECETED_DRAWER != hWh.DeviceType)return;
	Buffer[1] = 1;
	Buffer[3] = Place;
	Buffer[4] = Speed;
	Drawer_SendData(Buffer, 5);
}
void Drawer_Stop(void)
{
	unsigned char Buffer[4];
	if(WAREHOUSE_DECETED_DRAWER != hWh.DeviceType)return;
	Buffer[1] = 2;
	Drawer_SendData(Buffer, 3);
}
unsigned char Drawer_Status(unsigned char *Buffer, unsigned char Lenth)
{
	unsigned char buffer[4];
	if(WAREHOUSE_DECETED_DRAWER != hWh.DeviceType)return 1;
	buffer[1] = 3;
	if(Drawer_SendData(buffer, 3))
	{
		return 1;
	}
	return Drawer_Recive(Buffer, Lenth);
}
unsigned char Drawer_Vision(unsigned char *Buffer, unsigned char Lenth)
{
	unsigned char buffer[4];
	if(WAREHOUSE_DECETED_DRAWER != hWh.DeviceType)return 1;
	buffer[1] = 4;
	if(Drawer_SendData(buffer, 3))
	{
		return 1;
	}
	return Drawer_Recive(Buffer, Lenth);
}
