#ifndef __CHARGE_MANAGE_H__
#define __CHARGE_MANAGE_H__

#ifdef __cplusplus
extern "C"{
#endif


#ifdef __IO_H__

#define IO_CHARGE_ENABLE		PinCode(PORTE, 1<<4)
#define IO_CHARGE_CHA_ENABLE	PinCode(PORTE, 1<<6)
#define IO_CHARGE_CHB_ENABLE	PinCode(PORTE, 1<<5)

#define ADC_CHARGE_U_CH			1
#define ADC_CHARGE_I_CH			2
#define ADC_CHARGE_A_CH			3
#define ADC_CHARGE_B_CH			0
#define ADC_VBAT_CH				11
	
typedef struct
{
	IO_HandleTypeDef Enable;
	IO_HandleTypeDef ChannelA;
	IO_HandleTypeDef ChannelB;
	unsigned char Last;
}ChargeMamageHandle;


#endif

void CHG_Initilize(void);
unsigned long Charging(void* args);
void CHG_Control(unsigned char code);
unsigned int CHG_GetUA(void);
unsigned int CHG_GetUB(void);
unsigned int CHG_GetU(void);
unsigned int CHG_GetI(void);
unsigned int CHG_GetVBat(void);
#ifdef __cplusplus
}
#endif

#endif
