#include "stm32f1xx_hal.h"
#include "IO.h"
#include "Adc.h"
#include "motor.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
static void PID_Doing(int i);
static void Servo_GetArgs(int i);

void PID_Driver_Trig(void)
{
	hMotor.Trig.TimeBase = 1;
}
void PID_Driver(void)
{
	int i;
	short encoder;
	volatile pMoterChannelHandler motor = (pMoterChannelHandler)0;
	if(hMotor.Trig.TimeBase)
	{
		hMotor.Trig.TimeBase = 0;
		for(i=0; i<MOTOR_TOTAL_NUMBER; i++)
		{
			motor = &hMotor.Motor[i];
			if(0 == i)
			{
				encoder = (short)(motor->Hardware.Encoder.Instance->CNT);
				motor->Hardware.Encoder.Instance->CNT = 0;
				motor->MotorChannelArgs.EncoderResualt = encoder;
			}
			Servo_GetArgs(i);
			if(hMotor.Motor[i].MotorChannelArgs.Enable)
				PID_Doing(i);
		}
	}
}
void PID_Init(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->Enable = 0;								//使能
	
	args->SetTotal = 0;								//目标位移
	args->RealTotal = 0;							//现实位移
	args->EncoderResualt = 0;
	
	args->SetSpeed = 0;								//目标速度
	args->MinSpeed = 100;							//加减速过程控制中启动/停止速度
	args->CurrentSpeed = 0;							//当前目标速度
	args->RealSpeed = 0;							//现实速度
	args->SetAcceleration = 100;					//设定加速度
	args->RealAcceleration = 0;						//现实加速度
	
	args->MaxCurrent = 0;							//最大电流
	args->RealCurrent = 0;							//现实电流
	
	args->SpeedCirclePid.KP = 20;
	args->SpeedCirclePid.KI = 2;
	args->SpeedCirclePid.KD = 0.4;
	args->SpeedCirclePid.errILim = 100;
	args->TotalCirclePid.KP = 0.5;
	args->TotalCirclePid.KI = 0.05;
	args->TotalCirclePid.KD = 0.025;
	args->TotalCirclePid.errILim = 100;
	

	/*PID算法内部变量，其值不能修改*/
	args->SpeedCirclePid.errNow = 0;
	args->SpeedCirclePid.ctrOut = 0;
	args->SpeedCirclePid.errOld = 0;
	args->SpeedCirclePid.errP = 0;
	args->SpeedCirclePid.errI = 0;
	args->SpeedCirclePid.errD = 0;
	
	/*PID算法内部变量，其值不能修改*/
	args->TotalCirclePid.errNow = 0;
	args->TotalCirclePid.ctrOut = 0;
	args->TotalCirclePid.errOld = 0;
	args->TotalCirclePid.errP = 0;
	args->TotalCirclePid.errI = 0;
	args->TotalCirclePid.errD = 0;
}
static int PID_SetCurrentSpeed(int i)
{
	
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	volatile double dect, t, ss, ds = args->SetAcceleration / hMotor.Args.SpeedRateInHz;
	volatile __int64 LeftTotal = args->SetTotal - args->RealTotal;
	int Signt = SIGN(LeftTotal);
	
	if(LeftTotal > INT32_MAX)
		LeftTotal = INT32_MAX;
	if(LeftTotal < INT32_MIN)
		LeftTotal = INT32_MIN;
	
	args->SetSpeed = ABS(args->SetSpeed) * Signt;
	if(ABS(LeftTotal) < ABS(args->SetSpeed) / 36)
	{
		Servo_Stop(i);
		return 0;
	}
	t = (sqrt(2 * ABS(LeftTotal) * args->SetAcceleration + args->MinSpeed * args->MinSpeed)- args->MinSpeed) / args->SetAcceleration;
	ss = args->MinSpeed + t * args->SetAcceleration;
	ss = MMIN(ABS(args->SetSpeed), ss) * Signt;
	
	dect = ss - args->CurrentSpeed;
	args->CurrentSpeed = (ABS(dect)<ABS(ds)) ? ss : (args->CurrentSpeed + ds * SIGN(dect));
	args->CurrentSpeed = args->SetSpeed;
	args->PidSpeed = args->CurrentSpeed;
	args->CalTotal += args->CurrentSpeed / hMotor.Args.SpeedRateInHz;
	args->PidSpeed = args->CurrentSpeed/* + (ABS(args->CalTotal) - ABS(args->RealTotal)) * 1.0 * SIGN(args->CurrentSpeed)*/;
	return 1;
}
static void Servo_GetArgs(int i)
{
	int j;
	double sum = 0;
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	
	args->RealTotal += args->EncoderResualt;
	args->RealSpeed = args->EncoderResualt + args->RealSpeed * (hMotor.Args.SpeedRateInHz - 1) / hMotor.Args.SpeedRateInHz;
	for(j=0; j<19; j++)
	{
		sum += (args->PreEncoderResualt[j] = args->PreEncoderResualt[j+1]);
	}
	sum += (args->PreEncoderResualt[19] = args->EncoderResualt);
	args->RealSpeed = sum * 0.05 * hMotor.Args.SpeedRateInHz;
	args->Current = 3.0 * args->CurrentAdr / hMotor.Args.Aref / args->CurrentAdrIndex * 5;
	args->CurrentAdr = 0;
	args->CurrentAdrIndex = 0;
}
static void PID_Cal(PID * pid)
{
	pid->errP = pid->errNow;
	pid->errI = 0.5 * pid->errI + pid->errNow;
	pid->errD = pid->errNow - pid->errOld;
	pid->errOld = pid->errNow;
	
	if(fabs(pid->errILim) > FLT_EPSILON)
	{
		if(fabs(pid->errI) > fabs(pid->errILim)) pid->errI = fabs(pid->errILim) * fabs(pid->errI) / pid->errI;
		if(fabs(pid->errD) > fabs(pid->errILim)) pid->errD = fabs(pid->errILim) * fabs(pid->errD) / pid->errD;
	}
	pid->ctrOut += pid->KP * pid->errP + pid->KI * pid->errI + pid->KD * pid->errD;
	
	
}

static void PID_Doing(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	PID * spid = & args->SpeedCirclePid;
	//PID * tpid = & args->TotalCirclePid;
	if(PID_SetCurrentSpeed(i))
	{
		/*tpid->errNow = args->CalTotal - args->RealTotal;
		tpid->ctrOut = args->CurrentSpeed;
		PID_Cal(tpid);
		args->PidSpeed = tpid->ctrOut;*/
		
		spid->errNow = args->PidSpeed / hMotor.Args.SpeedRateInHz - args->EncoderResualt;
		PID_Cal(spid);
		if(fabs(spid->ctrOut) > hMotor.Hardware.PwmTimHandler.Init.Period)
			spid->ctrOut = hMotor.Hardware.PwmTimHandler.Init.Period * fabs(spid->ctrOut) / spid->ctrOut;
		Motor_SetOpenDirPwm(i, (int)spid->ctrOut);
	}
	
}
void Servo_Stop(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	
	args->RealTotal += args->EncoderResualt;
	
	Motor_SetOpenDirPwm(i, 0);
	args->CurrentSpeed = 0;
	args->SpeedCirclePid.ctrOut = 0;
	args->SpeedCirclePid.errP = args->SpeedCirclePid.errI = args->SpeedCirclePid.errD = 0;
	Motor_SetOpenDirPwm(i, 0);
	args->EncoderResualt = 0;
	args->CalTotal = args->RealTotal;
	if(0 == i)
	{
		IO_TogglePin(&hMotor.Motor[i].Hardware.Dir);
	}
	if(-5000 < args->RealTotal && args->RealTotal < 5000)
	{
		args->Enable = 0;
	}
	
}
void Servo_SetSpeed(int i, double rate)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->SetSpeed = rate;
}
double Servo_GetSpeed(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->SetSpeed;
}
double Servo_GetRealSpeed(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->RealSpeed;
}
void Servo_SetAcceleration(int i, double rate)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->SetAcceleration = rate;
}
double Servo_GetAcceleration(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->SetAcceleration;
}
void Servo_SetMinSpeed(int i, double rate)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->MinSpeed = rate;
}
double Servo_GetMinSpeed(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->MinSpeed;
}
void Servo_SetEnable(int i, int enable)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->Enable = enable;
	if(!enable)
	{
		Servo_Stop(i);
	}
	else
	{
		hMotor.Motor[i].Hardware.Encoder.Instance->CNT = 0;
	}
	
}
unsigned char Servo_GetEnable(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->Enable;
}
void Servo_SetTotal(int i, __int64 Total)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->SetTotal = Total;
}
__int64 Servo_GetTotal(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->SetTotal;
}
void Servo_MinusTotal(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->SetTotal -= args->RealTotal;
	args->RealTotal = 0;
	args->CalTotal = 0;
}
__int64 Servo_GetRealTotal(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->RealTotal;
}
double Servo_GetCalTotal(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->CalTotal;
}
double Servo_GetSpeedCircleP(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->SpeedCirclePid.KP;
}
void Servo_SetSpeedCircleP(int i, double P)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->SpeedCirclePid.KP = P;
}
double Servo_GetSpeedCircleI(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->SpeedCirclePid.KI;
}
void Servo_SetSpeedCircleI(int i, double I)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->SpeedCirclePid.KI = I;
}
double Servo_GetSpeedCircleD(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->SpeedCirclePid.KD;
}
void Servo_SetSpeedCircleD(int i, double D)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->SpeedCirclePid.KD = D;
}
double Servo_GetTotalCircleP(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->TotalCirclePid.KP;
}
void Servo_SetTotalCircleP(int i, double P)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->TotalCirclePid.KP = P;
}
double Servo_GetTotalCircleI(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->TotalCirclePid.KI;
}
void Servo_SetTotalCircleI(int i, double I)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->TotalCirclePid.KI = I;
}
double Servo_GetTotalCircleD(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->TotalCirclePid.KD;
}
void Servo_SetTotalCircleD(int i, double D)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	args->TotalCirclePid.KD = D;
}
unsigned int Servo_GetRealCurrent(int i)
{
	unsigned long adr = ADC_Read(0==i ? 5 : 6);
	adr *= 6875;
	adr /= 19;
	adr >>= 9;
	return (unsigned int)adr;
}
unsigned char Servo_GetInitilized(int i)
{
	pMoterChannelHandler motor = &hMotor.Motor[i];
	pMotorChannelArgsType args = &(motor->MotorChannelArgs);
	return args->Initilized;
}
