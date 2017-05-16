#include "stm32f1xx_hal.h"
#include "IO.h"
#include "motor.h"
#include <string.h>
#include "tinyos.h"
MotorHandler hMotor;
void Motor_SetPwm(int id, int Pwm)
{
	assert_param((0 <= id) && (id < MOTOR_TOTAL_NUMBER));
	if(Pwm < 0) Pwm = 0;
	if(Pwm > hMotor.Hardware.PwmTimHandler.Init.Period)
		Pwm = hMotor.Hardware.PwmTimHandler.Init.Period;
	hMotor.Motor[id].MotorChannelArgs.PwmOut = Pwm;
	hMotor.Motor[id].Hardware.PwmConfig.Pulse = hMotor.Motor[id].MotorChannelArgs.PwmOut;
	HAL_TIM_PWM_ConfigChannel(&hMotor.Hardware.PwmTimHandler, &hMotor.Motor[id].Hardware.PwmConfig, hMotor.Motor[id].Hardware.PwmChannel);
	HAL_TIM_PWM_Start(&hMotor.Hardware.PwmTimHandler, hMotor.Motor[id].Hardware.PwmChannel);
}
int Motor_GetPwm(int id)
{
	return hMotor.Motor[id].MotorChannelArgs.PwmOut;
}
static void Motor_Encoder_Init(TIM_HandleTypeDef * htim)
{
	TIM_Encoder_InitTypeDef sConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	
	
	htim->Init.Prescaler = 0;
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = 65535;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	HAL_TIM_Encoder_Init(htim, &sConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig);
	HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}
static void Motor_ArgsInit()
{
	Servo_SetMinSpeed(0, 100);
	Servo_SetAcceleration(0, 10000);
}
/*static void Motor_HeadLimit1Int(void);
static void Motor_HeadLimit2Int(void);*/
static void Motor_LeftLimit1Int(void);
static void Motor_LeftLimit2Int(void);
static void Motor_RightLimit1Int(void);
static void Motor_RightLimit2Int(void);
static void Motor_HeadEncoderInt(void);
static void Motor_LeftEncoderInt(void);
static void Motor_RightEncoderInt(void);
static void Motor0Cal(void);
void Motor_Initilize()
{
	int i;
	const unsigned long PinCodes[6][3] = {MOTOR_LIST(CONTROL_DIR),MOTOR_LIST(CONTROL_PWM),MOTOR_LIST(ENCODER_CH1),MOTOR_LIST(ENCODER_CH2),MOTOR_LIST(LIMIT_CH1),MOTOR_LIST(LIMIT_CH2)};
	const unsigned int PwmChannal[] = MOTOR_LIST(PWM_CHANNAL);
	const ExtiIrqCallbackDelegate ExitIrqCallback[][3] = {
		{Motor0Cal, Motor_LeftLimit1Int, Motor_RightLimit1Int},
		{Motor0Cal, Motor_LeftLimit2Int, Motor_RightLimit2Int},
		{Motor_HeadEncoderInt, Motor_LeftEncoderInt, Motor_RightEncoderInt}
	};
	
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_RCC_TIM7_CLK_ENABLE();
	
	memset(&hMotor, 0, sizeof(MotorHandler));
	hMotor.Args.Size = sizeof(MotorHandler);
	
	hMotor.Args.SpeedRateInHz = 100;
	hMotor.Hardware.PwmTimHandler.Instance = MOTOR_PWM_TIM;
	hMotor.Hardware.PwmTimHandler.Init.Period = 18000;
	hMotor.Hardware.PwmTimHandler.Init.Prescaler = 0;
	hMotor.Hardware.PwmTimHandler.Init.ClockDivision = 0;
	hMotor.Hardware.PwmTimHandler.Init.CounterMode = TIM_COUNTERMODE_UP;
	
	HAL_TIM_PWM_Init(&hMotor.Hardware.PwmTimHandler);
	HAL_NVIC_SetPriority(MOTOR_PWM_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(MOTOR_PWM_IRQn);
	
	hMotor.Args.SpeedRateInHz = 50;
	
	hMotor.Hardware.Vcc5HwEnable.PinId = MOTOR_5I_PIN;
	hMotor.Hardware.Vcc5HwEnable.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hMotor.Hardware.Vcc5HwEnable.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	IO_Init(&hMotor.Hardware.Vcc5HwEnable);
	Motor_5VCtrl(1);
	
	hMotor.Hardware.Vcc5BldcEnable.PinId = MOTOR_5W_PIN;
	hMotor.Hardware.Vcc5BldcEnable.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hMotor.Hardware.Vcc5BldcEnable.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	IO_Init(&hMotor.Hardware.Vcc5BldcEnable);
	Motor_Bldc5VCtrl(1);
	
	hMotor.Hardware.Vcc5HwOverload.PinId = MOTOR_5I_OVERFLOW;
	hMotor.Hardware.Vcc5HwOverload.Init.Mode = GPIO_MODE_INPUT;
	hMotor.Hardware.Vcc5HwOverload.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	hMotor.Hardware.Vcc5HwOverload.Init.Pull = GPIO_PULLDOWN;
	IO_Init(&hMotor.Hardware.Vcc5HwOverload);
	
	hMotor.Hardware.Vcc5BldcOverload.PinId = MOTOR_5W_OVERFLOW;
	hMotor.Hardware.Vcc5BldcOverload.Init.Mode = GPIO_MODE_INPUT;
	hMotor.Hardware.Vcc5BldcOverload.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	hMotor.Hardware.Vcc5BldcOverload.Init.Pull = GPIO_PULLDOWN;
	IO_Init(&hMotor.Hardware.Vcc5BldcOverload);
	
	hMotor.Hardware.Vcc24HwEnable.PinId = MOTOR_24V_ON_PIN;
	hMotor.Hardware.Vcc24HwEnable.Init.Mode = GPIO_MODE_OUTPUT_PP;
	hMotor.Hardware.Vcc24HwEnable.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	IO_Init(&hMotor.Hardware.Vcc24HwEnable);
	
	
	hMotor.Hardware.Vcc24HwLocked.PinId = MOTOR_24_LOCK_PIN;
	hMotor.Hardware.Vcc24HwLocked.Init.Mode = GPIO_MODE_INPUT;
	hMotor.Hardware.Vcc24HwLocked.Init.Speed = GPIO_SPEED_FREQ_HIGH;
	IO_Init(&hMotor.Hardware.Vcc24HwLocked);
	
	Motor_24VCtrl(1);
	
	for(i=0; i<3; i++)
	{
		PID_Init(i);
		
		hMotor.Motor[i].Hardware.Dir.PinId = PinCodes[0][i];
		hMotor.Motor[i].Hardware.Dir.Init.Mode = GPIO_MODE_OUTPUT_PP;
		hMotor.Motor[i].Hardware.Dir.Init.Speed = GPIO_SPEED_FREQ_HIGH;
		IO_Init(&hMotor.Motor[i].Hardware.Dir);
		
		hMotor.Motor[i].Hardware.PwmOut.PinId = PinCodes[1][i];
		hMotor.Motor[i].Hardware.PwmOut.Init.Mode = GPIO_MODE_AF_PP;
		hMotor.Motor[i].Hardware.PwmOut.Init.Speed = GPIO_SPEED_FREQ_HIGH;
		IO_Init(&hMotor.Motor[i].Hardware.PwmOut);
		hMotor.Motor[i].Hardware.PwmChannel = PwmChannal[i];
		hMotor.Motor[i].Hardware.PwmConfig.OCMode = TIM_OCMODE_PWM1;
		hMotor.Motor[i].Hardware.PwmConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
		hMotor.Motor[i].Hardware.PwmConfig.OCFastMode = TIM_OCFAST_DISABLE;
		Motor_SetOpenDirPwm(i, 0);
		
		hMotor.Motor[i].Hardware.Limit1.PinId = PinCodes[4][i];
		hMotor.Motor[i].Hardware.Limit1.Init.Mode = GPIO_MODE_IT_FALLING;
		if(0 == i)
		{
			hMotor.Motor[i].Hardware.Limit1.Init.Mode = GPIO_MODE_IT_RISING_FALLING;
		}
		hMotor.Motor[i].Hardware.Limit1.Init.Speed = GPIO_SPEED_FREQ_HIGH;
		hMotor.Motor[i].Hardware.Limit1.Init.Pull = GPIO_PULLUP;
		hMotor.Motor[i].Hardware.Limit1.ExtiIrqCallback = ExitIrqCallback[0][i];
		IO_Init(&hMotor.Motor[i].Hardware.Limit1);
		
		if(0 == i)
		{
			
			hMotor.Motor[i].Hardware.Encoder1.PinId = PinCodes[2][i];
			hMotor.Motor[i].Hardware.Encoder1.Init.Mode = GPIO_MODE_AF_INPUT;
			hMotor.Motor[i].Hardware.Encoder1.Init.Speed = GPIO_SPEED_FREQ_HIGH;
			hMotor.Motor[i].Hardware.Encoder1.Init.Pull = GPIO_PULLUP;
			IO_Init(&hMotor.Motor[i].Hardware.Encoder1);
			
			hMotor.Motor[i].Hardware.Encoder2.PinId = PinCodes[3][i];
			hMotor.Motor[i].Hardware.Encoder2.Init.Mode = GPIO_MODE_AF_INPUT;
			hMotor.Motor[i].Hardware.Encoder2.Init.Speed = GPIO_SPEED_FREQ_HIGH;
			hMotor.Motor[i].Hardware.Encoder2.Init.Pull = GPIO_PULLUP;
			IO_Init(&hMotor.Motor[i].Hardware.Encoder2);
			
			hMotor.Motor[i].Hardware.Encoder.Instance = TIM4;
			Motor_Encoder_Init(&hMotor.Motor[i].Hardware.Encoder);
			
			hMotor.Motor[i].Hardware.Limit2.PinId = PinCodes[5][i];
			hMotor.Motor[i].Hardware.Limit2.Init.Mode = GPIO_MODE_IT_RISING_FALLING;
			hMotor.Motor[i].Hardware.Limit2.Init.Speed = GPIO_SPEED_FREQ_HIGH;
			hMotor.Motor[i].Hardware.Limit2.Init.Pull = GPIO_PULLUP;
			hMotor.Motor[i].Hardware.Limit2.ExtiIrqCallback = ExitIrqCallback[1][i];
			IO_Init(&hMotor.Motor[i].Hardware.Limit2);
			
		}
		else
		{
			hMotor.Motor[i].Hardware.Encoder1.PinId = PinCodes[2][i];
			hMotor.Motor[i].Hardware.Encoder1.Init.Mode = GPIO_MODE_IT_RISING_FALLING;
			hMotor.Motor[i].Hardware.Encoder1.Init.Speed = GPIO_SPEED_FREQ_HIGH;
			hMotor.Motor[i].Hardware.Encoder1.Init.Pull = GPIO_PULLUP;
			hMotor.Motor[i].Hardware.Encoder1.ExtiIrqCallback = ExitIrqCallback[2][i];
			IO_Init(&hMotor.Motor[i].Hardware.Encoder1);
		}
	}
	Motor_ArgsInit();
}

	
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
		__HAL_AFIO_REMAP_TIM1_ENABLE();
	if(htim->Instance == TIM3)
		__HAL_AFIO_REMAP_TIM3_ENABLE();
}
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
		__HAL_AFIO_REMAP_TIM4_ENABLE();
}
void Motor_5VCtrl(unsigned char Enable)
{
	IO_WritePin(&hMotor.Hardware.Vcc5HwEnable, Enable);
}
void Motor_Bldc5VCtrl(unsigned char Enable)
{
	IO_WritePin(&hMotor.Hardware.Vcc5BldcEnable, Enable);
}
unsigned char Motor_Hw5vOverLoad(void)
{
	return IO_ReadPin(&hMotor.Hardware.Vcc5HwOverload);
}
unsigned char Motor_Bldc5vOverLoad(void)
{
	return IO_ReadPin(&hMotor.Hardware.Vcc5BldcOverload);
}
void Motor_24VCtrl(unsigned char Enable)
{
	IO_WritePin(&hMotor.Hardware.Vcc24HwEnable, Enable);
}
void Motor_SetOpenDirPwm(int id, int Pwm)
{
	unsigned int d = 0;
	if(!id)
		Pwm *= -1;
	if(Pwm <= 0)
	{
		d = 1;
		Pwm = - Pwm;
	}
	if(Pwm > 0 || id > 0)
	{
		IO_WritePin(&hMotor.Motor[id].Hardware.Dir, d);
	}
	Motor_SetPwm(id, Pwm);
}
void MOTOR_PWM_IRQFUN()
{
	HAL_TIM_IRQHandler(&hMotor.Hardware.PwmTimHandler);
}
/*static void Motor_HeadLimit1Int()
{
	Motor0Cal();
}
static void Motor_HeadLimit2Int()
{
	Motor0Cal();
}*/
static void Motor_LeftLimit1Int()
{
	hMotor.Motor[1].MotorChannelArgs.RealTotal = 0;
}
static void Motor_LeftLimit2Int(){}
static void Motor_RightLimit1Int()
{
	hMotor.Motor[2].MotorChannelArgs.RealTotal = 0;
}
static void Motor_RightLimit2Int(){}
static void Motor_HeadEncoderInt(void){}
static void Motor_LeftEncoderInt(void)
{
	if(IO_ReadPinBuffer(&hMotor.Motor[1].Hardware.Dir))
		hMotor.Motor[1].MotorChannelArgs.RealTotal--;
	else
		hMotor.Motor[1].MotorChannelArgs.RealTotal++;
	if(hMotor.Motor[1].MotorChannelArgs.RealTotal >= hMotor.Motor[1].MotorChannelArgs.SetTotal
		&& hMotor.Motor[1].MotorChannelArgs.PwmOut > 3000)
	{
		Motor_SetOpenDirPwm(2, 3000 + hMotor.Motor[1].MotorChannelArgs.SetTotal * 8);
	}
}
static void Motor_RightEncoderInt(void)
{
	if(IO_ReadPinBuffer(&hMotor.Motor[2].Hardware.Dir))
		hMotor.Motor[2].MotorChannelArgs.RealTotal--;
	else
		hMotor.Motor[2].MotorChannelArgs.RealTotal++;
	if(hMotor.Motor[2].MotorChannelArgs.RealTotal >= hMotor.Motor[2].MotorChannelArgs.SetTotal
		&& hMotor.Motor[2].MotorChannelArgs.PwmOut > 3000)
	{
		Motor_SetOpenDirPwm(2, 3000+ hMotor.Motor[2].MotorChannelArgs.SetTotal * 8);
	}
}
static void Motor0Cal(void)
{
	static unsigned int pre = 0;
	pMoterChannelHandler motor = &hMotor.Motor[0];
	motor->MotorChannelArgs.IntFlag =  IO_ReadPin(&motor->Hardware.Limit1) + 2 *IO_ReadPin(&motor->Hardware.Limit2);
	motor->MotorChannelArgs.IntFlag ^= 3;
	if(pre != motor->MotorChannelArgs.IntFlag)
	{
		printf("Int = %d\tpre = %d\t Step = %.0lf\r\n", motor->MotorChannelArgs.IntFlag, pre, motor->MotorChannelArgs.RealTotal);
	}
	if(pre == 0)
	{
		if(motor->MotorChannelArgs.IntFlag == 1)
		{
			if(!(motor->Hardware.Encoder.Instance->CR1 & 0x10))
			{
				Servo_Stop(0);
				if(motor->MotorChannelArgs.Initilized == 0)
				{
					if(motor->MotorChannelArgs.SetTotal >= 5100)
					{
						motor->MotorChannelArgs.CalTotal = motor->MotorChannelArgs.RealTotal = 5100;
						Task_Trig("HeadCheck", (void*)0, 0);
						motor->MotorChannelArgs.Initilized = 1;
						Servo_SetTotal(0, 0);
						motor->MotorChannelArgs.Enable = 1;
					}
				}
				else
				{
					motor->MotorChannelArgs.Enable = 0;
				}
			}
			else
			{
				motor->MotorChannelArgs.IntFlag = 0;
			}
		}
		else if(motor->MotorChannelArgs.IntFlag == 2)
		{
			if(motor->Hardware.Encoder.Instance->CR1 & 0x10)
			{
				//motor->MotorChannelArgs.IntFlag = 0;
				Servo_Stop(0);
				if(motor->MotorChannelArgs.Initilized == 0)
				{
					if(motor->MotorChannelArgs.SetTotal <-5400)
					{
						motor->MotorChannelArgs.CalTotal = motor->MotorChannelArgs.RealTotal = -5400;
						motor->MotorChannelArgs.Initilized = 1;
						Task_Trig("HeadCheck", (void*)0, 0);
						Servo_SetTotal(0, 0);
						motor->MotorChannelArgs.Enable = 1;
					}
				}
				else
				{
					motor->MotorChannelArgs.Enable = 0;
				}
			}
			else
			{
				motor->MotorChannelArgs.IntFlag = 0;
			}
		}
		else if(motor->MotorChannelArgs.IntFlag == 3)
		{
			if(motor->Hardware.Encoder.Instance->CR1 & 0x10)
			{
				motor->MotorChannelArgs.CalTotal = motor->MotorChannelArgs.RealTotal = 165;
			}
			else
			{
				motor->MotorChannelArgs.CalTotal = motor->MotorChannelArgs.RealTotal =  -165;
			}
			if(motor->MotorChannelArgs.Initilized == 0)
			{
				Task_Trig("HeadCheck", (void*)0, 0);
				motor->MotorChannelArgs.Initilized = 1;
				Servo_SetTotal(0, 0);
				motor->MotorChannelArgs.Enable = 1;
			}
		}
	}
	pre = motor->MotorChannelArgs.IntFlag;
}
