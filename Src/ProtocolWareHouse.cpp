#include "COM.h"
#include "Pad.h"
#include "Protocol.h"
#include "WareHouse.h"
#include "Sliding.h"
static unsigned char preSequence[]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
void Protocol_DealFun_WareHouse(ProtocolInfo* pi)
{
	unsigned char Ack[2] = {0};
	ProtocolInfo Info(WAREHOUSE_MODULE, SRC_MODULE(pi->Module));
	
	Info.Action = 0xA1;
	Ack[0] = pi->Sequence;
	Ack[1] = pi->Action;
	Info.LinkParameterList(Ack, 2);
	Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
	Info.Send();
	switch(pi->Action)
	{
		case 0x01:
			Sliding_PowerControl(pi->ParameterList[0]);
			break;
		case 0x0f:
			WH_DeviceChangeCallback(WH_Deceted());
			break;
		case 0x11:
			if(pi->ParameterList[0]<70 && pi->ParameterList[1]<11)
			{
				Drawer_Move(pi->ParameterList[0], pi->ParameterList[1]);
			}
			break;
		case 0x12:
			Drawer_Stop();
			break;
	}
}
void Protocol_UploadFun_WareHouse(void)
{
	unsigned char device = WH_Deceted();
	unsigned char Buffer[9];
	ProtocolInfo Info(WAREHOUSE_MODULE, PAD_MODULE);
	if(WAREHOUSE_DECETED_DRAWER == device)
	{
		if(0 == Drawer_Status(Buffer, 8))
		{
			Info.Action = 0x15;
			Info.LinkParameterList(Buffer+3, 5);
			Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
			Info.Send();
		}
	}
}
void WH_DeviceChangeCallback(unsigned char NewDevice)
{
	unsigned char Buffer[40];
	ProtocolInfo Info(WAREHOUSE_MODULE, PAD_MODULE);
	
	Info.Action = 0x0f;
	Buffer[0]= NewDevice;
	Info.LinkParameterList(Buffer, 1);
	Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
	Info.Send();
	switch(NewDevice)
	{
		case WAREHOUSE_DECETED_DRAWER:
			if(0 == Drawer_Vision(Buffer, 37))
			{
				Buffer[2] = 0x01;
				Info.Action = 0x1f;
				Info.LinkParameterList(Buffer + 3, 33);
				Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
				Info.Send();
			}
			break;
	}
}
