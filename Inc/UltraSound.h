#ifndef __ULTRASOUND_H__
#define __ULTRASOUND_H__
#ifdef __cplusplus
extern "C"{
#endif
	#ifdef __IO_H__
	
	#define ULTRASOUND_PWON			PinCode(PORTA,  1<<12)
	#define ULTRASOUND_PWOV			PinCode(PORTA,  1<<11)
	
	typedef struct
	{
		IO_HandleTypeDef PowerControl;
		IO_HandleTypeDef PowerOverload;
	}UltraSoundHandle;
	#endif
	void UltraSound_Initilize(void);
	void UltraSound_PowerControl(unsigned char OnOff);
	unsigned char UltraSound_PowerSate(void);
	unsigned char UltraSound_PowerOverload(void);
#ifdef __cplusplus
}
#endif

#endif
