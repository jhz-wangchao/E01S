#include "COM.h"
#include "Protocol.h"

static unsigned char preSequence[]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
void Protocol_DealFun_Projector(ProtocolInfo* pi)
{
	unsigned char Ack[2] = {0};
	ProtocolInfo Info(PROJECTION_MODULE, SRC_MODULE(pi->Module));
	
	Info.Action = 0xA1;
	Ack[0] = pi->Sequence;
	Ack[1] = pi->Action;
	Info.LinkParameterList(Ack, 2);
	Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
	Info.Send();
	
	switch(pi->Action)
	{
		case 0x01:
			COMS[COM5]->Transmits(pi->ParameterList, pi->ParaLen);
			break;
	}
	
}
unsigned long Projector_Reciving(void* args)
{
	static unsigned char RecivingBuffer[256] = {0};
	static int index = -1;
	unsigned char data;
	
	while(COMS[COM5]->GetRecivedDataSize() > 0)
	{
		data = COMS[COM5]->GetRecivedData();
		if(0xAA == data)
		{
			RecivingBuffer[0] = 0xAA;
			index = 1;
		}
		else if(index >= 0)
		{
			RecivingBuffer[index++] = data;
			if(index > 256)index = -1;
			if(0xFF == data)
			{
				ProtocolInfo Info(PROJECTION_MODULE, PAD_MODULE);
				Info.Action = 0x01;
				Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
				Info.LinkParameterList(RecivingBuffer, index);
				Info.Send();
				index = -1;
			}
		}
	}
	return 0;
}
