#include "COM.h"
#include "Protocol.h"
#include "Projector.h"

void Protocol_DealFun_Sliding(ProtocolInfo* pi)
{
	static unsigned char preSequence[]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	unsigned char Ack[2] = {0}, Buffer[3] = {0};
	ProtocolInfo Info(HEADWING_MODULE, SRC_MODULE(pi->Module));
	
	Info.Action = 0xA1;
	Ack[0] = pi->Sequence;
	Ack[1] = pi->Action;
	Info.LinkParameterList(Ack, 2);
	Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
	Info.Send();
	
	switch(pi->Action)
	{
		case 0x01:
			Projector_PowerControl(pi->ParameterList[0]);
			break;
		case 0x02:
			Projector_Move(pi->ParameterList[0]);
			break;
		case 0x03:
			Buffer[0] = Projector_PowerSate();
			Buffer[1] = Projector_DourStatus();
			
			Info.Action = 0x03;
			Info.LinkParameterList(Buffer, 3);
			Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
			Info.Send();
			break;
		
	}
	
}
