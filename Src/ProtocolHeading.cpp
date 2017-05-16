#include<iostream>
#include<string>
#include<cstdlib>
#include "COM.h"
#include "Protocol.h"
#include "tinyos.h"
#include "main.h"
#include "Motor.h"
static void Servo_TimeOut(int Id, int TimeOut);
static int Initilized = 0;
static unsigned char preSequence[]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
void Protocol_DealFun_Heading(ProtocolInfo* pi)
{
	
	unsigned char Ack[2];
	ProtocolInfo Info(HEADWING_MODULE, SRC_MODULE(pi->Module));
	int Total;
	int Speed;
	int i;
	
	Info.Action = 0xA1;
	Ack[0] = pi->Sequence;
	Ack[1] = pi->Action;
	Info.LinkParameterList(Ack, 2);
	Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
	Info.Send();
	
	switch(pi->Action)
	{
		case 0x01:
			Total = ((int)((short)(pi->ParameterList[0] * 256 + pi->ParameterList[1])));
			Total = 300 - Total;
			Total = Total * 36 - 5400;
			Speed = ((int)(pi->ParameterList[2])) * 36;
			Servo_SetSpeed(0, Speed);
			Servo_SetTotal(0, Total);
			Servo_SetEnable(0, 1);
			break;
		case 0x11:
			for(i=0; i<2; i++)
			{
				if(pi->ParameterList[2*i+0] > 0)
				{
					Servo_SetTotal(2-i, pi->ParameterList[2*i+0]);
					Motor_SetOpenDirPwm(2-i, pi->ParameterList[2*i+1] * 40 + 3500);
					Servo_TimeOut(2-i, 5000);
				}
			}
			break;
		default:
			break;
	}
}
void Protocol_UploadFun_Heading(void)
{
	unsigned char Buffer[12] = {0};
	ProtocolInfo Info(HEADWING_MODULE, PAD_MODULE);
	int i;
	
	int HeadAngle = 0, LeftAngle = 0, RightAngle = 0, HeadCurrent = 0, WingCurrent = 0, MovingState = 0, ErrorCode = 0;
	
	if(!Initilized)
	{
		return;
	}
	HeadAngle	= (Servo_GetRealTotal(0) + 5400) / 36;
	HeadAngle   = 300 - HeadAngle;
	LeftAngle	= Servo_GetRealTotal(1)>0?Servo_GetRealTotal(1):0;
	RightAngle	= Servo_GetRealTotal(2)>0?Servo_GetRealTotal(2):0;
	HeadCurrent	= Servo_GetRealCurrent(0);
	WingCurrent	= Servo_GetRealCurrent(1);
	
	for(i=0;i<3;i++)
	{
		if(Motor_GetPwm(i) > 0)
		{
			MovingState |= (1<<i);
		}
	}
	if(HeadCurrent > 2000) ErrorCode |= 2;
	if(WingCurrent > 2000) ErrorCode |= 4;
	if(0 > HeadAngle || HeadAngle > 300) ErrorCode |= 8;
	if(LeftAngle > 100)ErrorCode |= 16;
	if(RightAngle > 100)ErrorCode |= 32;
	
	Buffer[0] = (unsigned char)((HeadAngle >> 8) & 0xff);
	Buffer[1] = (unsigned char)(HeadAngle & 0xff);
	Buffer[2] = (unsigned char)(LeftAngle & 0xff);
	Buffer[3] = (unsigned char)(RightAngle & 0xff);
	Buffer[4] = (unsigned char)((HeadCurrent >> 8) & 0xff);
	Buffer[5] = (unsigned char)(HeadCurrent & 0xff);
	Buffer[6] = (unsigned char)((WingCurrent >> 8) & 0xff);
	Buffer[7] = (unsigned char)(WingCurrent & 0xff);
	Buffer[8] = (unsigned char)((MovingState >> 8) & 0xff);
	Buffer[9] = (unsigned char)(MovingState & 0xff);
	Buffer[10] = (unsigned char)((ErrorCode >> 8) & 0xff);
	Buffer[11] = (unsigned char)(ErrorCode & 0xff);
	
	Info.Action = 0x32;
	Info.LinkParameterList(Buffer, 12);
	Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
	Info.Send();
}
void Protocol_UploadFun_Check(void)
{
	unsigned char Buffer[13] = {0};
	ProtocolInfo Info(HEADWING_MODULE, PAD_MODULE);
	
	int ErrorCode = 0, WingCrurrent = 0, HeadCrurrent = 0, LeftRealTotal = 0, RightRealTotal = 0, HeadRealTotal = 0;
	
	WingCrurrent   = Servo_GetRealCurrent(1);
	HeadCrurrent   = Servo_GetRealCurrent(0);
	LeftRealTotal  = (unsigned char)(Servo_GetRealTotal(1)>0?Servo_GetRealTotal(1):0);
	RightRealTotal = (unsigned char)(Servo_GetRealTotal(2)>0?Servo_GetRealTotal(2):0);
	HeadRealTotal = (short)(Servo_GetRealTotal(0));
	
	if(WingCrurrent  > 1500)ErrorCode |= 1;
	if(HeadCrurrent  > 2500)ErrorCode |= 2;
	if(LeftRealTotal  > 110)ErrorCode |= 4;
	if(RightRealTotal > 110)ErrorCode |= 8;
	if(-5400 > HeadRealTotal || HeadRealTotal > 5400) ErrorCode |= 16;
	
	Buffer[0] = (unsigned char)ErrorCode;
	Buffer[2] = (unsigned char)((WingCrurrent >> 8) & 0xff);
	Buffer[3] = (unsigned char)(WingCrurrent & 0xff);
	Buffer[4] = (unsigned char)((HeadCrurrent >> 8) & 0xff);
	Buffer[5] = (unsigned char)(HeadCrurrent & 0xff);
	Buffer[6] = (unsigned char)(LeftRealTotal & 0xff);
	Buffer[7] = (unsigned char)(RightRealTotal & 0xff);
	Buffer[8] = (unsigned char)((HeadRealTotal >> 8) & 0xff);
	Buffer[9] = (unsigned char)(HeadRealTotal & 0xff);
	
	Info.Action = 0x30;
	Info.LinkParameterList(Buffer, 13);
	Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
	Info.Send();
	Initilized = 1;
}
static void Servo_TimeOut(int Id, int TimeOut)
{
	Task_Delete("WingTimeout", (void*)Id, TimeOut);
	Task_Trig("WingTimeout", (void*)Id, TimeOut);
}
