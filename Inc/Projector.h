#ifndef __PROJECTOR_H__
#define __PROJECTOR_H__
#ifdef __cplusplus
extern "C"{
#endif

	#define PROJECT_DOUR_CLOSE			0
	#define PROJECT_DOUR_OPEN			1
	#define PROJECT_DOUR_MOVEING		2
	#define PROJECT_DOUR_TIMEOUT		3
	#ifdef __IO_H__
	
	#define PROJECTOR_TIMX				TIM7
	#define PROJECTOR_CLOCK_ENABLE()	__HAL_RCC_TIM7_CLK_ENABLE()
	#define PROJECTOR_IRQn				TIM7_IRQn
	#define PROJECTOR_IRQHandler		TIM7_IRQHandler
	
	
	#define PROJECTOR_UP_CTRL		PinCode(PORTE,  1<<3)
	#define PROJECTOR_DOWN_CTRL		PinCode(PORTE,  1<<2)
	#define PROJECTOR_PWON			PinCode(PORTC,  1<<14)
	/*#define SLIDING_24_ON			PinCode(PORTD,  1<<0)
	#define SLIDING_5_ON			PinCode(PORTC,  1<<7)*/
	
	#define PROJECTOR_UP_DECETE		PinCode(PORTB,  1<<3)
	#define PROJECTOR_DOWN_DECETE	PinCode(PORTB,  1<<4)
	/*#define SLIDING_5_OVERLOAD	PinCode(PORTC,  1<<6)
	#define SLIDING_ADC_DECETE	PinCode(PORTC,  1<<3)*/
	
	typedef struct
	{
		TIM_HandleTypeDef TimeBaseHandle;
		
		IO_HandleTypeDef UpControl;
		IO_HandleTypeDef DownControl;
		IO_HandleTypeDef PowerControl;
		
		IO_HandleTypeDef UpDecete;
		IO_HandleTypeDef DownDecete;
		
		unsigned char Dir;
		unsigned char Status;
		unsigned int  TimeCount;
	}ProjectorHandle;
	#endif
	void Projector_Initilize(void);
	void Projector_PowerControl(unsigned char OnOff);
	unsigned char Projector_PowerSate(void);
	void Projector_Move(unsigned int Dir);
	int Projector_DourStatus(void);
	int Projector_UGet(void);
	int Projector_IGet(void);
#ifdef __cplusplus
}
#endif

#endif
