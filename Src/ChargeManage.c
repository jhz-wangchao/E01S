#include "stm32f1xx_hal.h"
#include "IO.h"
#include "ADC.h"
#include "ChargeManage.h"
ChargeMamageHandle hChg;
void CHG_Initilize(void)
{
	const unsigned long PinId[] = {IO_CHARGE_ENABLE, IO_CHARGE_CHA_ENABLE, IO_CHARGE_CHB_ENABLE};
	pIO_HandleTypeDef pIo = &hChg.Enable;
	int i;
	
	for(i=0; i<3; i++,pIo++)
	{
		pIo->PinId = PinId[i];
		pIo->Init.Speed = GPIO_SPEED_FREQ_HIGH;
		pIo->Init.Mode = GPIO_MODE_OUTPUT_PP;
		IO_Init(pIo);
	}
	CHG_Control(0);
}
unsigned long Charging(void* args)
{
	unsigned int ua = CHG_GetUA(), ub = CHG_GetUB(), u = CHG_GetU(),vbat = CHG_GetVBat();
	unsigned char a = (ua > 0xF00?1:0), b = (ub > 0xF00?1:0);
	CHG_Control((b<<1) | a);
	CHG_GetI();
	/*if(CHG_GetI() >200)
	{
		IO_WritePin(&hChg.Enable, 0);
	}*/
	return 0;
}
void CHG_Control(unsigned char code)
{
	hChg.Last = code;
	switch(code)
	{
		case 0:
			IO_WritePin(&hChg.Enable, 0);
			IO_WritePin(&hChg.ChannelA, 0);
			IO_WritePin(&hChg.ChannelB, 0);
			break;
		case 1:
			IO_WritePin(&hChg.Enable, 1);
			IO_WritePin(&hChg.ChannelA, 1);
			IO_WritePin(&hChg.ChannelB, 0);
			break;
		case 2:
			IO_WritePin(&hChg.Enable, 1);
			IO_WritePin(&hChg.ChannelA, 0);
			IO_WritePin(&hChg.ChannelB, 1);
			break;
		default:
			CHG_Control(0);
			break;
	}
}
unsigned int CHG_GetUA(void)
{
	return ADC_Read(ADC_CHARGE_A_CH);
}
unsigned int CHG_GetUB(void)
{
	return ADC_Read(ADC_CHARGE_B_CH);
}
unsigned int CHG_GetU(void)
{
	return (ADC_Read(ADC_CHARGE_U_CH) * 21 * 3300) >>12;
}
unsigned int CHG_GetI(void)
{
	return ADC_Read(ADC_CHARGE_I_CH)<<2;
}
unsigned int CHG_GetVBat(void)
{
	return (ADC_Read(ADC_VBAT_CH) * 21 * 3300) >>12;
}
