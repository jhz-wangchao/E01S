#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C"{
#endif

#ifdef __STM32F1xx_HAL_H

#define ADC_CHANNEL_TOTAL	3
	
#define ADC_PIN_CH0			PinCode(PORTC,  1<<2)
#define ADC_CHANNEL_CH0		ADC_CHANNEL_12
#define ADC_PIN_CH1			PinCode(PORTC,  1<<0)
#define ADC_CHANNEL_CH1		ADC_CHANNEL_10
#define ADC_PIN_CH2			PinCode(PORTC,  1<<1)
#define ADC_CHANNEL_CH2		ADC_CHANNEL_11

#define ADC_SELECT_PIN_0	PinCode(PORTE,  1<<1)
#define ADC_SELECT_PIN_1	PinCode(PORTE,  1<<0)
#define ADC_SELECT_PIN_2	PinCode(PORTB,  1<<9)
typedef struct
{
	IO_HandleTypeDef			Io;
	ADC_InjectionConfTypeDef	JConfig;
	//ADC_ChannelConfTypeDef		sConfig;
}AdcChannel;

typedef struct
{
	ADC_HandleTypeDef			Adc;
	IO_HandleTypeDef			Select[3];
	AdcChannel					Channels[3];
	unsigned char 				SelectIndex;
	unsigned char				AdrIndex;
	unsigned int				Adr[17][16];
}AdcHandle, *pAdcHandle;

#endif

void ADC_Initilize(void);
unsigned int ADC_Read(unsigned char Channel);
#ifdef __cplusplus
}
#endif
#endif
