#include <string.h>
#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "IO.h"
#include "ADC.h"
static AdcHandle HAdc;
static void Adc_ChangeIndex(unsigned char Index)
{
	int i;
	Index &= 0x07;
	HAdc.SelectIndex = Index;
	for(i=0; i<3; i++,Index>>=1)
	{
		IO_WritePin(&HAdc.Select[i], Index & 0x01);
	}
}
void ADC_Initilize(void)
{
	int i;
	const unsigned int PinId[] = {ADC_SELECT_PIN_0, ADC_SELECT_PIN_1, ADC_SELECT_PIN_2};
	const unsigned int AdcCh[] = {ADC_CHANNEL_CH0, ADC_CHANNEL_CH1, ADC_CHANNEL_CH2};
	
	HAdc.Adc.Instance = ADC1;
	
	HAdc.Adc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	HAdc.Adc.Init.ContinuousConvMode = ENABLE;
	HAdc.Adc.Init.DiscontinuousConvMode = DISABLE;
	HAdc.Adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAdc.Adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	HAdc.Adc.Init.NbrOfConversion = 2;
	
	HAdc.Channels[0].Io.PinId = ADC_PIN_CH0;
	HAdc.Channels[1].Io.PinId = ADC_PIN_CH1;
	HAdc.Channels[2].Io.PinId = ADC_PIN_CH2;
	HAL_ADC_Init(&HAdc.Adc);
	
	for(i=0; i<3; i++)
	{
		HAdc.Channels[i].JConfig.InjectedNbrOfConversion = 3;
		HAdc.Channels[i].JConfig.InjectedChannel = AdcCh[i];
		HAdc.Channels[i].JConfig.InjectedRank = 1 + i;
		HAdc.Channels[i].JConfig.InjectedSamplingTime = ADC_SAMPLETIME_239CYCLES_5;
		HAdc.Channels[i].JConfig.InjectedOffset = 0;
		HAdc.Channels[i].JConfig.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
		HAdc.Channels[i].JConfig.AutoInjectedConv = ENABLE;
		HAdc.Channels[i].JConfig.InjectedDiscontinuousConvMode = DISABLE;
		HAL_ADCEx_InjectedConfigChannel(&HAdc.Adc, &HAdc.Channels[i].JConfig);
		
	}
	/*for(i=0; i<2; i++)
	{
		HAdc.Channels[i].sConfig.Channel = AdcCh[i];
		HAdc.Channels[i].sConfig.Rank = 1 + i;
		HAdc.Channels[i].sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
		HAL_ADC_ConfigChannel(&HAdc.Adc, &HAdc.Channels[i].sConfig);
	}*/
	for(i=0; i<3; i++)
	{
		HAdc.Select[i].PinId = PinId[i];
		HAdc.Select[i].Init.Mode = GPIO_MODE_OUTPUT_PP;
		HAdc.Select[i].Init.Speed = GPIO_SPEED_FREQ_HIGH;
		IO_Init(&HAdc.Select[i]);
		IO_WritePin(&HAdc.Select[i], 0);
	}
	HAL_NVIC_SetPriority(ADC1_2_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
	HAL_ADCEx_InjectedStart_IT(&HAdc.Adc);
	Adc_ChangeIndex(0);
	HAL_ADC_Start_IT(&HAdc.Adc);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	HAdc.Channels[0].Io.Init.Mode = GPIO_MODE_ANALOG;
	HAdc.Channels[1].Io.Init.Mode = GPIO_MODE_ANALOG;
	HAdc.Channels[2].Io.Init.Mode = GPIO_MODE_ANALOG;
	IO_Init(&HAdc.Channels[0].Io);
	IO_Init(&HAdc.Channels[1].Io);
	IO_Init(&HAdc.Channels[2].Io);
}
static int compar(const void * a, const void * b)
{
	return *((unsigned int*)a) - *((unsigned int*)b);
}
unsigned int ADC_Read(unsigned char Channel)
{
	unsigned int adr[16];
	
	memcpy(adr, HAdc.Adr[Channel], 16 * sizeof(unsigned int));
	qsort(adr, 16, sizeof(unsigned int), compar);
	return adr[7];
}
void ADC1_2_IRQHandler()
{
	HAL_ADC_IRQHandler(&HAdc.Adc);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
}
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAdc.Adr[16][HAdc.AdrIndex] = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
	HAdc.Adr[HAdc.SelectIndex + 0][HAdc.AdrIndex] = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_2);
	HAdc.Adr[HAdc.SelectIndex + 8][HAdc.AdrIndex] = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_3);
	
	if(HAdc.SelectIndex == 0)
	{
		HAdc.AdrIndex = (HAdc.AdrIndex + 1) & 0x0f;
	}
	Adc_ChangeIndex(HAdc.SelectIndex + 1);
	HAL_ADCEx_InjectedStart_IT(&HAdc.Adc);
}
