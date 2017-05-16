//#include "stm32f1xx_hal.h"
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <list.h>
#include "Projector.h"
#include "Pad.h"
#include "Sliding.h"
#include "UltraSound.h"
#include "IR.h"
#include "COM.h"
#include "ChargeManage.h"
#include "IIC.h"
#include "WareHouse.h"
#include "IO.h"
#include "TinyOs.h"
#include "Led.h"
#include "Log.h"
#include "Power.h"
#include "Protocol.h"
#include "Motor.h"
#include "main.h"
#include "Adc.h"
#include "stm32f1xx_hal.h"


extern "C" unsigned int HAL_GetTick(void);
extern "C"void HAL_Delay(volatile unsigned int Delay);
using namespace std;

unsigned long Initilize(void* args);
unsigned long PowerMng(void* args);
unsigned long MotorDriver(void* args);
unsigned long WingDriver(void * args);
unsigned long HeadCheck(void* args);
unsigned long OverloadCheck(void* args);
unsigned long WareHouse(void* args);
unsigned long FeedDog(void* args);
IWDG_HandleTypeDef hIwdg;

#if OS_PIN_MONITOR
TaskInitTypedef TaskInfo[] = {
	//名称			跟踪PORT		跟踪PIN			优先级		间隔时间	入口		参数		任务模式
	{"Init",		PinCode(PORTC, 1<<2),			5,			0,			Initilize,	(void*)0,	TaskRunSingle},
	{"PowerMng",		PinCode(PORTC, 1<<4),			1,			1,			PowerMng,	(void*)0,	TaskRunTimming},
	{(char*)0}
};
#else
TaskInitTypedef const TaskInfo[] = {
	//名称			优先级		间隔时间	入口				参数		任务模式
	{"Init",		5,			0,			Initilize,			(void*)0,	TaskRunSingle},
	{"PowerMng",	1,			1,			PowerMng,			(void*)0,	TaskRunTimming},
	{"Protocol",	2,			0,			Protocol_Deal,		(void*)0,	TaskRunByEvent},
	{"ProtocolUp",	5,			200,		Protocol_Up,		(void*)0,	TaskRunTimming},
	{"MotorDrv",	1,			20,			MotorDriver,		(void*)0,	TaskRunTimming},
	{"WingTimeout",	4,			0,			WingDriver,			(void*)0,	TaskRunByEvent},
	{"HeadCheck",	2,			0,			HeadCheck,			(void*)0,	TaskRunByEvent},
	{"Projector",	10,			100,		Projector_Reciving,	(void*)0,	TaskRunTimming},
	{"Overload",	10,			100,		OverloadCheck,		(void*)0,	TaskRunTimming},
	{"Charging",	9,			20,			Charging,			(void*)0,	TaskRunTimming},
	{"WareHouse",	9,			500,		WareHouse,			(void*)0,	TaskRunTimming},
	{"FeedDog",		13,			300,		FeedDog,			(void*)0,	TaskRunTimming},
	{(char*)0}
};
#endif
void AddProtocolTask(const char * Name, MODULE module);
int main()
{
	int i;
	Os_Initilize();
	
	for(i=0; TaskInfo[i].Name;i++)
	{
		Task_Add((pTaskInitTypedef)(&TaskInfo[i]));
	}
	HAL_Delay(50);
	Os_Run();
}
unsigned long Initilize(void* args)
{
	COMS[COM4]->Initilize();
	COMS[COM5]->Initilize();
	Power_Initilize();
	
	Log::Infomation("System Boot Success");
	#ifdef __DEBUG__
	Log::Infomation("Debug Mode");
	#endif
	
	AddProtocolTask("PadReciver", PAD_MODULE);
	AddProtocolTask("UltReciver", ULT_MODULE);
	AddProtocolTask("InfReciver", INFRARED_MODULE);
	//AddProtocolTask("ProReciver", PROJECTION_MODULE);
	ADC_Initilize();
	
	Pad_Initilize();
	Pad_BeepControl(1);
	Motor_Initilize();
	Projector_Initilize();
	
	Sliding_Initilize();
	UltraSound_Initilize();
	Ir_Initilize();
	CHG_Initilize();
	
	/*Servo_SetTotal(1, 60);
	Motor_SetOpenDirPwm(1, 4000);
	Task_Trig("WingTimeout", (void*)1, 2000);
	Servo_SetTotal(2, 60);
	Motor_SetOpenDirPwm(2, 4000);
	Task_Trig("WingTimeout", (void*)2, 2000);*/
	Pad_PowerControl(1);
	Sliding_PowerControl(1);
	Pad_AudioControl(1);
	UltraSound_PowerControl(1);
	Ir_PowerControl(1);
	//Projector_PowerControl(1);
	
	hIwdg.Instance = IWDG;
	hIwdg.Init.Prescaler = IWDG_PRESCALER_256;
	hIwdg.Init.Reload = 0xfff;
	HAL_IWDG_Init(&hIwdg);
	HAL_IWDG_Start(&hIwdg);
	HAL_IWDG_Refresh(&hIwdg);
	
	Servo_SetSpeed(0, 1000);
	Servo_SetTotal(0, -100000);
	Servo_SetEnable(0, 1);
	Pad_BeepControl(0);
	return 0;
	
}
void AddProtocolTask(const char * Name, MODULE module)
{
	TaskInitTypedef ti = {Name,	1,			2,			Protocol_Reciving,	(void*)0,	TaskRunTimming};
	ProtocolStaticArgs * psa = new ProtocolStaticArgs(module);
	ti.Args = psa;
	psa->InitProtocolCom();
	Task_Add(&ti);
}
unsigned long PowerMng(void* args)
{
	static volatile unsigned int keyOffTime = 0;
	static volatile unsigned char Off = 0;
	if(Power_KeyDetect())
	{
		if(keyOffTime > 500 )
		{
			if(Off)
			{
				keyOffTime = HAL_GetTick();
			}
			else if(HAL_GetTick() - keyOffTime > 200)
			{
				Off = 1;
				keyOffTime = HAL_GetTick();
			}
		}
	}
	else
	{
		if(Off)
		{
			if(HAL_GetTick() - keyOffTime > 100)
			{
				Log::Infomation("关机");
				HAL_Delay(200);
				Power_Off();
			}
		}
		else
			keyOffTime = HAL_GetTick();
	}
	return 0;
}
unsigned long MotorDriver(void* args)
{
	PID_Driver_Trig();
	PID_Driver();
	return 0;
}
unsigned long WingDriver(void * args)
{
	Motor_SetOpenDirPwm((int)args, 0);
	return 0;
}
unsigned long HeadCheck(void* args)
{
	void Protocol_UploadFun_Check(void);
	Protocol_UploadFun_Check();
	return 0;
}
unsigned long OverloadCheck(void* args)
{
	/*if((Servo_GetRealCurrent(0) > 2000) || (Servo_GetRealCurrent(1) > 2000))
	{
		Motor_24VCtrl(0);
		Motor_5VCtrl(0);
		Motor_Bldc5VCtrl(0);
		Servo_Stop(0);
		Servo_Stop(1);
		Servo_Stop(2);
	}*/
	if((Projector_UGet() > 15000) || (Projector_IGet() > 6000))
	{
		Projector_PowerControl(0);
	}
	if(UltraSound_PowerOverload() && UltraSound_PowerSate())
	{
		UltraSound_PowerControl(0);
	}
	if(Ir_PowerOverload() && Ir_PowerSate())
	{
		Ir_PowerControl(0);
	}
	/*if(ADC_Read(9) > 750)//I > 6A
	{
		Log::Infomation("关机");
		HAL_Delay(200);
		Power_Off();
	}*/
	return 0;
}
unsigned long WareHouse(void* args)
{
	static unsigned char step = 0;
	const unsigned char s[] = {1, 3, 1, 3};
	Sliding_PowerControl_24(s[step]>>1);
	Sliding_PowerControl_5(s[step] & 0x01);
	step = (step + 1) & 0x03;
	return 0;
}
unsigned long FeedDog(void* args)
{
	HAL_IWDG_Refresh(&hIwdg);
	return 0;
}
