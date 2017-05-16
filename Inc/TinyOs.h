#ifndef __TINYOS_H__
#define __TINYOS_H__
#ifdef __cplusplus
extern "C"{
#endif
#include "List.h"
#define OS_TIM_BASE_TIMX			TIM6
#define OS_TIM_BASE_CLOCK_ENABLE()	__HAL_RCC_TIM6_CLK_ENABLE()
#define OS_TIM_BASE_IRQn			TIM6_IRQn
#define OS_TIM_BASE_IRQHandler		TIM6_IRQHandler
#define OS_MAX_TASK_NUM				64
#define OS_MAX_EVENT_NUM			64


	
#define OS_PIN_MONITOR				0
#define OS_IDLE_SLEEP				0
#define OS_TIMESPAN					1
#if OS_TIMESPAN > 65
#error TinyOs timespan must be smaller than 66 ms.
#endif
#if OS_PIN_MONITOR
#define CPU_WORK_PIN				PinCode(PORTC, GPIO_PIN_0)
#define CPU_INTERRUPT_PIN			PinCode(PORTC, GPIO_PIN_1)
#endif
typedef enum
{
	TaskReady=0,
	TaskRunning,
	TaskBlock,
	TaskWait,
	TaskIdle,
}TaskState;

typedef enum
{
	TaskRunSingle = 0,
	TaskRunTimming,
	TaskRunNomal,
	TaskRunByEvent,
}TaskRunMode;
typedef struct _Event
{
	struct _Event *Next;
	unsigned long TrigTime;
	void * Args;
}Event, *pEvent;
typedef struct
{
	const char * Name;
	#if OS_PIN_MONITOR
	unsigned long	TaskLookPinId;
	#endif
	unsigned long	Priority;		//任务优先级
	unsigned long	Timming;		//任务间隔时间
	unsigned long	(*Run)(void*);	//任务运行函数
	void*			Args;			//任务参数列表
	
	TaskRunMode		TaskMode;		//任务状态
}TaskInitTypedef, *pTaskInitTypedef;
#ifdef __STM32F1xx_HAL_H
typedef struct _TASK
{
	
	char			Name[32];		//任务名称
	#if OS_PIN_MONITOR
	IO_HandleTypeDef   TaskLookPin;	//当前任务运行时，下两个参数指示的引脚会高电平
	#endif
	unsigned long	Priority;		//任务优先级
	unsigned long	Timming;		//任务间隔时间
	unsigned long	(*Run)(void*);	//任务运行函数
	void*			Args;			//任务参数列表
	
	TaskRunMode		TaskMode;		//任务状态
	TaskState		TaskState;		//任务
	
	unsigned long 	RunCount;		//运行次数
	unsigned long	RunElapsed;		//运行总耗时(微秒)
	unsigned long	LastRunTime;	//最近一次运行时间(微秒)
	unsigned long	LastRunElapsed;	//最近一次运行耗时(微秒)
	
	
}Task, *pTask;



typedef struct
{
	TIM_HandleTypeDef				TimeBaseHandle;
	volatile unsigned __int64 		TimeTick;
	volatile unsigned __int64 		InterruptMs,InterruptUs;
	#if OS_PIN_MONITOR
	IO_HandleTypeDef	CpuWorkPin;
	#endif
	volatile unsigned long CpuWorkUsPerhms[16];
	List Tasks;
	pTask CurrentTask;
}OsHandle, *pOsHandle;
#else
typedef void *pTask;
#endif
#define NullTask		((pTask)0)
void Os_Initilize(void);
unsigned char Task_Add(pTaskInitTypedef ti);
unsigned char Task_Trig(const char* taskName, void * args, unsigned long timeDelay);
unsigned char Task_Delete(const char * taskName, void * args, unsigned long timeDelay);
unsigned __int64 Os_GetOsTimeTick(void);
unsigned __int64 Os_GetOsTimeWork(void);
unsigned __int64 Os_GetOsTimeWorkLastSecond(void);

void Os_Run(void);
void OS_EnterInterruptMode(void);
void OS_ExitInterruptMode(void);
pTask Os_CurrentTask(void);
void Os_TaskAbout(pTask pt);
#ifdef __cplusplus
}
#endif
#endif
