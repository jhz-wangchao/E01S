#ifndef __MOTOR_H__
#define __MOTOR_H__
#ifdef __cplusplus
extern "C"{
#endif
	#define MOTOR_PWM_TIM			TIM1
	#define MOTOR_PWM_IRQn			TIM1_CC_IRQn
	#define MOTOR_PWM_IRQFUN		TIM1_CC_IRQHandler

	#define MOTOR_TIME_BASE_TIM		TIM7
	#define MOTOR_TIME_BASE_IRQn	TIM7_IRQn
	#define MOTOR_TIME_BASE_IRQFUN	TIM7_IRQHandler
	
	#define MOTOR_5I_PIN			PinCode(PORTA,  1<<3)
	#define MOTOR_5W_PIN			PinCode(PORTA,  1<<1)
	#define MOTOR_24V_ON_PIN		PinCode(PORTC,  1<<3)
	#define MOTOR_24_LOCK_PIN		PinCode(PORTB,  1<<12)
	#define MOTOR_5I_OVERFLOW		PinCode(PORTA,  1<<2)
	#define MOTOR_5W_OVERFLOW		PinCode(PORTA,  1<<4)
	
	#define MOTOR_1_CONTROL_DIR		PinCode(PORTE,  1<<12)
	#define MOTOR_1_CONTROL_PWM		PinCode(PORTE,  1<<13)		//TIM1_CH3 AFIO
	#define MOTOR_1_PWM_CHANNAL		TIM_CHANNEL_3
	#define MOTOR_1_ENCODER_CH1		PinCode(PORTD,  1<<12)		//TIM4_CH1 AFIO
	#define MOTOR_1_ENCODER_CH2		PinCode(PORTD,  1<<13)		//TIM4_CH2 AFIO
	#define MOTOR_1_LIMIT_CH1		PinCode(PORTD,  1<<14)
	#define MOTOR_1_LIMIT_CH2		PinCode(PORTD,  1<<15)
	
	#define MOTOR_2_CONTROL_DIR		PinCode(PORTE,  1<<10)
	#define MOTOR_2_CONTROL_PWM		PinCode(PORTE,  1<<11)		//TIM1_CH2 AFIO
	#define MOTOR_2_PWM_CHANNAL		TIM_CHANNEL_2
	#define MOTOR_2_ENCODER_CH1		PinCode(PORTB,  1<<0)		
	#define MOTOR_2_ENCODER_CH2		PinCode(PORTB,  0)		
	#define MOTOR_2_LIMIT_CH1		PinCode(PORTC,  1<<4)
	#define MOTOR_2_LIMIT_CH2		PinCode(PORTC,  0)
	
	#define MOTOR_3_CONTROL_DIR		PinCode(PORTE,  1<<8)
	#define MOTOR_3_CONTROL_PWM		PinCode(PORTE,  1<<9)		//TIM1_CH1 AFIO
	#define MOTOR_3_PWM_CHANNAL		TIM_CHANNEL_1
	#define MOTOR_3_ENCODER_CH1		PinCode(PORTB,  1<<1)
	#define MOTOR_3_ENCODER_CH2		PinCode(PORTB,  0)
	#define MOTOR_3_LIMIT_CH1		PinCode(PORTC,  1<<5)
	#define MOTOR_3_LIMIT_CH2		PinCode(PORTC,  0)
	
	#ifndef MOTOR_TOTAL_NUMBER
	#define MOTOR_TOTAL_NUMBER 	 	3
	#endif
	
	#define MOTOR_LIST(W)			{MOTOR_1_##W, MOTOR_2_##W, MOTOR_3_##W}
#ifdef __STM32F1xx_HAL_H
typedef struct
{
	double		KP;
	double		KI;
	double		KD;
	
	double		errNow;				//当前的误差
	double		ctrOut;				//控制量输出

	double		errOld;
	double		errP;
	double		errI;
	double		errD;
	double		errILim;				//误差积分上限
}PID;
typedef struct
{
	PID           SpeedCirclePid;
	PID           TotalCirclePid;

	
	unsigned char Enable;				//使能
	unsigned char IntFlag;				//中断标识
	unsigned char Initilized;			//初始化完成标识
	int PwmOut;
	
	__int64       SetTotal;				//目标位移
	double        CalTotal;				//计算中的位移
	double        RealTotal;			//现实位移
	long          EncoderResualt;
	
	double        PreEncoderResualt[20];
	
	double        SetSpeed;				//目标速度
	double        MinSpeed;				//加减速过程控制中启动/停止速度
	double        CurrentSpeed;         //当前目标速度
	double        PidSpeed;
	double        RealSpeed;			//现实速度
	double        SetAcceleration;		//设定加速度
	double        RealAcceleration;		//现实加速度
	
	double        MaxCurrent;			//最大电流
	double        RealCurrent;          //现实电流
	


	int           CurrentAdr;
	int           CurrentAdrIndex;
	double        Current;
}MotorChannelArgsType, *pMotorChannelArgsType;
typedef struct
{
	struct
	{
		unsigned int PwmChannel;
		TIM_OC_InitTypeDef PwmConfig;
		
		TIM_HandleTypeDef Encoder;
		
		IO_HandleTypeDef Dir;
		IO_HandleTypeDef PwmOut;
		IO_HandleTypeDef Encoder1;
		IO_HandleTypeDef Encoder2;
		IO_HandleTypeDef Limit1;
		IO_HandleTypeDef Limit2;
		
		ADC_InjectionConfTypeDef CurrentAdcChannelConf;
		IO_HandleTypeDef CurrentAdcIo;
	}Hardware;
	MotorChannelArgsType MotorChannelArgs;
}MoterChannelHandler, *pMoterChannelHandler;
typedef struct
{
	struct
	{
		TIM_HandleTypeDef PwmTimHandler;
		
		IO_HandleTypeDef Vcc24HwEnable;
		IO_HandleTypeDef Vcc24HwLocked;
		
		IO_HandleTypeDef Vcc5HwEnable;
		IO_HandleTypeDef Vcc5BldcEnable;
		IO_HandleTypeDef Vcc5HwOverload;
		IO_HandleTypeDef Vcc5BldcOverload;
	}Hardware;
	struct
	{
		long Size;
		int SpeedRateInHz;
		int Aref;
	}Args;
	struct
	{
		unsigned char TimeBase;
	}Trig;
	MoterChannelHandler Motor[MOTOR_TOTAL_NUMBER];
}MotorHandler;
extern MotorHandler hMotor;
void PID_Init(int i);
void Motor_SetPwm(int id, int Pwm);
#endif
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define SIGN(a) ((a) > 0 ? 1 : -1)
#define MAX(type)				type(((unsigned type)(-1)) >> 1)
#define MIN(type)				type((((unsigned type)(-1)) >> 1) + 1)
#define MMAX(a, b) (((a)>(b))?(a):(b))
#define MMIN(a, b) (((a)<(b))?(a):(b))
void Motor_Initilize(void);
void PID_Driver_Trig(void);
void PID_Driver(void);
void Motor_SetOpenDirPwm(int id, int Pwm);
int Motor_GetPwm(int id);
void Servo_SetSpeed(int i, double rate);
double Servo_GetSpeed(int i);
double Servo_GetRealSpeed(int i);
void Servo_SetAcceleration(int i, double rate);
double Servo_GetAcceleration(int i);
void Servo_SetMinSpeed(int i, double rate);
double Servo_GetMinSpeed(int i);
void Servo_SetEnable(int i, int enable);
unsigned char Servo_GetEnable(int i);
void Servo_SetTotal(int i, __int64 Total);
__int64 Servo_GetTotal(int i);
void Servo_Stop(int i);
void Servo_MinusTotal(int i);
__int64 Servo_GetRealTotal(int i);
double Servo_GetCalTotal(int i);

double Servo_GetSpeedCircleP(int i);
void Servo_SetSpeedCircleP(int i, double P);
double Servo_GetSpeedCircleI(int i);
void Servo_SetSpeedCircleI(int i, double I);
double Servo_GetSpeedCircleD(int i);
void Servo_SetSpeedCircleD(int i, double D);
double Servo_GetTotalCircleP(int i);
void Servo_SetTotalCircleP(int i, double P);
double Servo_GetTotalCircleI(int i);
void Servo_SetTotalCircleI(int i, double I);
double Servo_GetTotalCircleD(int i);
void Servo_SetTotalCircleD(int i, double D);

unsigned int Servo_GetRealCurrent(int i);

unsigned char Servo_GetInitilized(int i);
void Motor_5VCtrl(unsigned char Enable);
void Motor_24VCtrl(unsigned char Enable);
void Motor_Bldc5VCtrl(unsigned char Enable);
unsigned char Motor_Hw5vOverLoad(void);
unsigned char Motor_Bldc5vOverLoad(void);

#ifdef __cplusplus
}
#endif
#endif
