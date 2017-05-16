#ifndef __SLIDING_H__
#define __SLIDING_H__
#ifdef __cplusplus
extern "C"{
#endif
	#ifdef __IO_H__
	
	#define SLIDING_PWON			PinCode(PORTD,  1<<0)
	#define SLIDING_PWON2			PinCode(PORTC,  1<<7)
	
	typedef struct
	{
		IO_HandleTypeDef PowerControl[2];
	}SlidingHandle;
	#endif

	void Sliding_Initilize(void);
	void Sliding_PowerControl(unsigned char OnOff);
	unsigned char Sliding_PowerSate(void);
	void Sliding_PowerControl_24(unsigned char OnOff);
	void Sliding_PowerControl_5(unsigned char OnOff);
#ifdef __cplusplus
}
#endif

#endif
