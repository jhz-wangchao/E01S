/*****************************************************************************
Copyright:  进化者
File name: TinyOs.c
Description: 提供简单的任务调度(非操作系统)
Author: wangchao
Version: V0.1
Date: 2017年1月24日
History: 
2017年2月14日	王超	增加对中断计时的支持
2017年2月14日	王超	STM32F1版本
2017年2月24日	王超	利用链表管理任务
2017年4月5日	王超	添加删除事件的逻辑
*****************************************************************************/
#include "stm32f1xx_hal.h"
#include "IO.h"
#include "TinyOs.h"
#include "string.h"
#include "stdlib.h"
OsHandle hOs;
static unsigned long Os_Task_Idle(void* args){(void)args; return 0;}
static void Task_Copy(pTask task, pTaskInitTypedef ti);
void OS_MspInit(void);
/****************************************************
	函数名:	Os_Initilize
	功能:	初始化任务序列与其使用的硬件资源
	作者:	wangchao 2017年1月24日10:14:26
****************************************************/
void Os_Initilize(void)
{
	//int i;
	TaskInitTypedef InterruptTask =
	{
		"INT",
		#if OS_PIN_MONITOR
		CPU_INTERRUPT_PIN,
		#endif
		255,
		0,
		Os_Task_Idle,
		(void*)0,
		TaskRunByEvent
	};
	OS_MspInit();
	List_Initilize(&hOs.Tasks);
	Task_Add(&InterruptTask);
}
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

	/**Configure the Systick interrupt time 
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick 
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
/****************************************************
	函数名:	OS_MspInit
	功能:	初始化任务序列使用的硬件资源
	作者:	wangchao 2017年1月24日10:14:26
****************************************************/
__weak void OS_MspInit(void)
{
	OS_TIM_BASE_CLOCK_ENABLE();
	
	
	HAL_Init();
	/*SystemClock_Config();
	HAL_InitTick(TICK_INT_PRIORITY);*/
	
	hOs.TimeBaseHandle.Instance = OS_TIM_BASE_TIMX;
	hOs.TimeBaseHandle.Init.Period = 1000 * OS_TIMESPAN;
	hOs.TimeBaseHandle.Init.Prescaler = HAL_RCC_GetPCLK2Freq()/1000000 - 1;
	hOs.TimeBaseHandle.Init.ClockDivision = 0;
	hOs.TimeBaseHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&hOs.TimeBaseHandle);
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	HAL_NVIC_SetPriority(OS_TIM_BASE_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(OS_TIM_BASE_IRQn);
	
	#if OS_PIN_MONITOR
	hOs.CpuWorkPin.PinId = CPU_WORK_PIN;
	hOs.CpuWorkPin.Init.Speed = GPIO_SPEED_HIGH;
	hOs.CpuWorkPin.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hOs.CpuWorkPin.Init.Pull = GPIO_NOPULL;
	IO_Init(&hOs.CpuWorkPin);
	IO_WritePin(&hOs.CpuWorkPin, GPIO_PIN_RESET);
	#endif
}
void HAL_MspInit(void)
{
	
	SystemClock_Config();
	HAL_InitTick(TICK_INT_PRIORITY);

}
void SysTick_Handler()
{
	HAL_IncTick();
}
void OS_TIM_BASE_IRQHandler()
{
	HAL_TIM_IRQHandler(&hOs.TimeBaseHandle);
}
__weak void HAL_TIM_PeriodElapsedCallbackNext(TIM_HandleTypeDef *htim)
{
	(void)htim;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(&hOs.TimeBaseHandle == htim)
	{
		hOs.TimeTick += OS_TIMESPAN;
	}
	else
	{
		HAL_TIM_PeriodElapsedCallbackNext(htim);
	}
}
/****************************************************
	函数名:	Task_Add
	功能:	向调度序列中添加任务信息
	参数:   ti: 任务初始化信息列表
	返回值: 1代表添加成功，0代表任务序列满，没有添加成功
	作者:	wangchao 2017年1月24日10:14:26
****************************************************/
unsigned char Task_Add(pTaskInitTypedef ti)
{
	pTask tmp = malloc(sizeof(Task));
	Task_Copy(tmp, ti);
	List_Add_Tal(&hOs.Tasks, tmp);
	return 1;
}
/****************************************************
	函数名:	Task_Copy
	功能:	任务初始化
	作者:	wangchao 2017年1月24日10:14:26
****************************************************/
static void Task_Copy(pTask task, pTaskInitTypedef ti)
{
	strcpy(task->Name, ti->Name);
	task->Priority = ti->Priority;
	task->Timming = ti->Timming;
	task->TaskMode = ti->TaskMode;
	task->Timming = ti->Timming;
	task->Run = ti->Run;
	task->Args = ti->Args;
	if(TaskRunTimming == task->TaskMode)
	{
		task->TaskState = TaskWait;
	}
	else if(TaskRunByEvent == task->TaskMode)
	{
		task->TaskState = TaskBlock;
		task->Args = (void*)0;
	}
	else
	{
		task->TaskState = TaskReady;
	}
	
	task->RunCount = 0;
	task->RunElapsed = 0;
	task->LastRunTime = hOs.TimeTick;
	task->LastRunElapsed = 0;
	#if OS_PIN_MONITOR
	
	if(ti->TaskLookPinId & 0xffff)
	{
		task->TaskLookPin.PinId = ti->TaskLookPinId;
		task->TaskLookPin.Init.Mode = GPIO_MODE_OUTPUT_PP;
		task->TaskLookPin.Init.Pull = GPIO_NOPULL;
		task->TaskLookPin.Init.Speed = GPIO_SPEED_HIGH;
		IO_Init(&task->TaskLookPin);
		IO_WritePin(&task->TaskLookPin, GPIO_PIN_RESET);
	}
	#endif
}
/****************************************************
	函数名:	Task_Trig
	功能:	触发事件型任务
	参数:	taskName:  事件名称
			args:      事件参数
			timeDelay: 事件触发延时
	返回值: 1代表触发成功，0代表触发失败
	作者:	wangchao 2017年1月24日13:47:54
****************************************************/
unsigned char Task_Trig(const char* taskName, void * args, unsigned long timeDelay)
{
	pListNode p;
	void**q;
	for(p = hOs.Tasks.Head; NULL_NODE != p; p = p->Next)
	{
		pTask t = (pTask)p->Buffer;
		if(strcmp(t->Name, taskName)==0 && t->TaskMode == TaskRunByEvent)
		{
			pEvent event = (pEvent)malloc(sizeof(Event));
			event->Next = (pEvent)0;
			event->TrigTime = hOs.TimeTick + timeDelay;
			event->Args = args;
			q = &t->Args;
			while(1)
			{
				if(!(*q))break;
				if(((pEvent)*q)->TrigTime > event->TrigTime)break;
				q = (void **)&(((pEvent)*q)->Next);
			}
			event->Next = (pEvent)*q;
			*q = event;
			return 1;
		}
	}
	return 0;
}
/****************************************************
	函数名:	Task_Delete
	功能:	删除指定名称、指定参数与时间段的时间
	参数:	taskName:  事件名称
			args:      事件参数
			timeDelay: 事件触发延时时限
	返回值: 1代表删除成功，0代表删除失败
	作者:	wangchao 2017年4月5日10:30:22
****************************************************/
unsigned char Task_Delete(const char * taskName, void * args, unsigned long timeDelay)
{
	pListNode p;
	void**q;
	pEvent event;
	for(p = hOs.Tasks.Head; NULL_NODE != p; p = p->Next)
	{
		pTask t = (pTask)p->Buffer;
		if(strcmp(t->Name, taskName)==0 && t->TaskMode == TaskRunByEvent)
		{
			q = &t->Args;
			while(*q)
			{
				event = (pEvent)(*q);
				if(event->Args == args && event->TrigTime <= hOs.TimeTick + timeDelay)
				{
					*q = event->Next;
					free(event);
				}
				else
				{
					q = (void**)&event->Next;
				}
			}
		}
	}
	return 0;
}

/****************************************************
	函数名:	Os_GetOsTimeTick
	功能:	获得系统调度总时间 微秒
	作者:	wangchao 2017年1月24日14:50:26
****************************************************/
unsigned __int64 Os_GetOsTimeTick(void)
{
	return hOs.TimeTick * hOs.TimeBaseHandle.Init.Period + hOs.TimeBaseHandle.Instance->CNT;
}
/****************************************************
	函数名:	Os_GetOsTimeWork
	功能:	获得CPU使用时间 微秒
	作者:	wangchao 2017年1月24日14:50:26
****************************************************/
unsigned __int64 Os_GetOsTimeWork(void)
{
	unsigned long sum = 0;
	pListNode p;
	for(p = hOs.Tasks.Head; NULL_NODE != p; p = p->Next)
	{
		pTask t = (pTask)p->Buffer;
		sum += t->RunElapsed;
	}
	return sum;
}
unsigned __int64 Os_GetOsTimeWorkLastSecond(void)
{
	int i;
	unsigned long sum = 0;
	for(i=0; i<16; i++)
	{
		sum += hOs.CpuWorkUsPerhms[i];
	}
	return sum;
}
pTask Os_CurrentTask(void)
{
	return hOs.CurrentTask;
}
void Os_TaskAbout(pTask pt)
{
	pListNode cur;
	for(cur = hOs.Tasks.Head; cur; cur = cur->Next)
	{
		if(cur->Buffer == pt)
		{
			List_Remove(&hOs.Tasks, cur);
			break;
		}
	}
}
static pTask Os_FindReadyHighestTask(void)
{
	pTask rtn = NullTask;
	int MaxPriority = 0;
	pListNode p;
	volatile pTask t;
	for(p = hOs.Tasks.Head; NULL_NODE != p; p = p->Next)
	{
		t = (pTask)p->Buffer;
		if(TaskReady == t->TaskState && t->Priority  > MaxPriority)
		{
			MaxPriority = t->Priority;
			rtn = t;
		}
	}
	return rtn;
}
/****************************************************
	函数名:	Os_Run
	功能:	调度任务
	作者:	wangchao 2017年1月24日10:14:26
****************************************************/
void Os_Run(void)
{
	int c, l;
	volatile unsigned __int64 current = 0, last = 0;
	unsigned long delay = 0;
	pListNode p;
	__int64 StartMs;
	__int64 StartUs;
	__int64 CpuWakeupMs;
	__int64 CpuWakeupUs;
	pTask t = NullTask;
	HAL_TIM_Base_Start_IT(&hOs.TimeBaseHandle);
	while(1)
	{
		current = hOs.TimeTick;
		#if OS_PIN_MONITOR
		IO_WritePin(&hOs.CpuWorkPin, GPIO_PIN_SET);
		#endif
		CpuWakeupMs = hOs.TimeTick;
		CpuWakeupUs = hOs.TimeBaseHandle.Instance->CNT;
		c = (current >> 6) & 0x0f;
		l = (last >> 6) & 0x0f;
		while(c!=l)
		{
			l = (l + 1) & 0x0f;
			hOs.CpuWorkUsPerhms[l] = 0;
			
		}
		for(p = hOs.Tasks.Head; NULL_NODE != p; p = p->Next)
		{
			t = (pTask)p->Buffer;
			
			//时间管理
			if((TaskRunTimming == t->TaskMode) && (TaskWait == t->TaskState))
			{
				if(hOs.TimeTick >= t->LastRunTime + t->Timming)
					t->TaskState = TaskReady;
			}
			if((TaskRunByEvent == t->TaskMode) && t->Args)
			{
				if(hOs.TimeTick >= ((pEvent)t->Args)->TrigTime)
				{
					t->TaskState = TaskReady;
				}
			}
		}
		while((t = Os_FindReadyHighestTask()) != NullTask)
		{
			//运行
			hOs.CurrentTask = t;
			if(TaskReady == t->TaskState)
			{
				#if OS_PIN_MONITOR
				IO_WritePin(&t->TaskLookPin, GPIO_PIN_SET);
				#endif
				/*hOs.TimeBaseHandle.Instance->CNT = 0;
				HAL_TIM_Base_Start(&hOs.TimeBaseHandle);*/
				StartMs = hOs.TimeTick;
				StartUs = hOs.TimeBaseHandle.Instance->CNT;
				if(TaskRunByEvent == t->TaskMode)
				{
					pEvent event = (pEvent)t->Args;
					t->Args = event->Next;
					delay = t->Run(event->Args);
					free(event);
				}
				else
				{
					delay = t->Run(t->Args);
				}
				StartMs = hOs.TimeTick - StartMs;
				StartUs = hOs.TimeBaseHandle.Instance->CNT - StartUs;
				/*HAL_TIM_Base_Stop(&hOs.TimeBaseHandle);*/
				#if OS_PIN_MONITOR
				IO_WritePin(&t->TaskLookPin, GPIO_PIN_RESET);
				#endif
				t->LastRunTime = hOs.TimeTick + delay;
				t->LastRunElapsed = StartMs * hOs.TimeBaseHandle.Init.Period + StartUs;
				t->RunElapsed += t->RunCount;
				t->RunElapsed += t->LastRunElapsed;
				
				switch(t->TaskMode)
				{
					case TaskRunSingle:
						t->TaskState = TaskIdle;
						break;
					case TaskRunTimming:
						t->TaskState = TaskWait;
						break;
					case TaskRunNomal:
						t->TaskState = TaskReady;
						break;
					case TaskRunByEvent:
						t->TaskState = TaskBlock;
						break;
					default:
						break;
				}
			}
		}
		CpuWakeupMs = hOs.TimeTick - CpuWakeupMs;
		CpuWakeupUs = hOs.TimeBaseHandle.Instance->CNT - CpuWakeupUs;
		hOs.CpuWorkUsPerhms[c] += CpuWakeupMs * hOs.TimeBaseHandle.Init.Period + CpuWakeupUs;
		#if OS_PIN_MONITOR
		IO_WritePin(&hOs.CpuWorkPin, GPIO_PIN_RESET);
		#endif
		//while(current == hOs.TimeTick);
		last = current;
		while(current + OS_TIMESPAN > hOs.TimeTick)
		{
			#if OS_IDLE_SLEEP
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
			#endif
		}
	}
}
void OS_EnterInterruptMode(void)
{
	#if OS_PIN_MONITOR
	pTask t = (pTask)hOs.Tasks.Head->Buffer;
	IO_WritePin(&t->TaskLookPin, GPIO_PIN_SET);
	#endif
	hOs.InterruptMs = hOs.TimeTick;
	hOs.InterruptUs = hOs.TimeBaseHandle.Instance->CNT;
}
void OS_ExitInterruptMode(void)
{
	pTask t = (pTask)hOs.Tasks.Head->Buffer;
	t->RunCount++;
	t->LastRunElapsed += (hOs.TimeTick - hOs.InterruptMs) * hOs.TimeBaseHandle.Init.Period + hOs.TimeBaseHandle.Instance->CNT - hOs.InterruptUs;
	t->LastRunTime = hOs.TimeTick;
	t->RunElapsed += t->LastRunElapsed;
	#if OS_PIN_MONITOR
	IO_WritePin(&t->TaskLookPin, GPIO_PIN_RESET);
	#endif
}
