#ifndef __IR_H__
#define __IR_H__
#ifdef __cplusplus
extern "C"{
#endif
	#ifdef __IO_H__
	
	#define IR_PWON			PinCode(PORTE,  1<<14)
	#define IR_PWOV			PinCode(PORTE,  1<<15)
	
	typedef struct
	{
		IO_HandleTypeDef PowerControl;
		IO_HandleTypeDef PowerOverload;
	}IrHandle;
	#endif
	void Ir_Initilize(void);
	void Ir_PowerControl(unsigned char OnOff);
	unsigned char Ir_PowerSate(void);
	unsigned char Ir_PowerOverload(void);
#ifdef __cplusplus
}
#endif

#endif
