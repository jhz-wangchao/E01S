#ifndef __WAREHOUSE_H__
#define __WAREHOUSE_H__
#ifdef __cplusplus
extern "C"{
#endif
	#define ADC_WAREHOUSE_DECETED_CHANNEL	16

	#define ADC_VALUE_CLEARER				0
	#define WAREHOUSE_DECETED_CLEARER		0

	#define ADC_VALUE_DRAWER				960
	#define WAREHOUSE_DECETED_DRAWER		1

	#define ADC_VALUE_SUBWOOFER				2048
	#define WAREHOUSE_DECETED_SUBWOOFER		2

	#define WAREHOUSE_DECETED_UNKNOWN		0xff

	#define WAREHOUSE_COMPARE_ERROR			80
	#define WAREHOUSE_MAX(a, b)				((a)>(b)?(a):(b))
	#define WAREHOUSE_MIN(a, b)				((a)<(b)?(a):(b))

	#define WAREHOUSE_MATCH(a, b)			((WAREHOUSE_MAX((b) - WAREHOUSE_COMPARE_ERROR, 0) < (a)) && ((a) < WAREHOUSE_MIN((b) + WAREHOUSE_COMPARE_ERROR, 4096)))
	#ifdef __IO_H__
	
	#define DRAWER_IIC						IIC1
	#define DRAWER_IIC_ADD					40
	#define DRAWER_INT_PIN					PinCode(PORTB, 1<<5)
	
	#define AIRCLEAR_PWM_TIMX				TIM3
	#define AIRCLEAR_PWM_PIN				DRAWER_INT_PIN
	typedef struct
	{
		IO_HandleTypeDef IntPin;
		pIIC_Driver Driver;
	}DrawerHandle;
	typedef struct
	{
		unsigned char DeviceType;
		DrawerHandle Drawer;
	}WareHouseHandle;

	#endif
	unsigned char WH_Deceted(void);
	void WH_DeviceChange(unsigned char NewDevice);
	
	void Drawer_Initilize(void);
	void Drawer_DeInitilize(void);
	void Drawer_Move(unsigned char Place, unsigned char Speed);
	void Drawer_Stop(void);
	unsigned char Drawer_Status(unsigned char *Buffer, unsigned char Lenth);
	unsigned char Drawer_Vision(unsigned char *Buffer, unsigned char Lenth);
	void WH_DeviceChangeCallback(unsigned char NewDevice);
#ifdef __cplusplus
}
#endif
#endif
