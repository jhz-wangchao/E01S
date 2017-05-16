#ifndef __PAD_H__
#define __PAD_H__
#ifdef __cplusplus
extern "C"{
#endif
	#ifdef __IO_H__
	
	#define PAD_PWON			PinCode(PORTC,  1<<15)
	#define PAD_AUDIO_CTRL		PinCode(PORTD,  1<<4)
	#define BEEP_CONTROL		PinCode(PORTD,  1<<1)
	#define PAD_STOP			PinCode(PORTA,  1<<0)
	
	typedef struct
	{
		IO_HandleTypeDef PowerControl;
		IO_HandleTypeDef AudioControl;
		IO_HandleTypeDef StopControl;
		IO_HandleTypeDef BeepControl;
	}PadHandle;
	#endif
	void Pad_Initilize(void);
	void Pad_PowerControl(unsigned char OnOff);
	unsigned char Pad_PowerSate(void);
	
	void Pad_AudioControl(unsigned char OnOff);
	void Pad_BeepControl(unsigned char OnOff);
	unsigned char Pad_AudioSate(void);
	
#ifdef __cplusplus
}
#endif

#endif
