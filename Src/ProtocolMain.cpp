#include<iostream>
#include<string>
#include<cstdlib>
#include "COM.h"
#include "Protocol.h"
#include "tinyos.h"
#include "main.h"
using namespace std;

void Protocol_DealFun_Main(ProtocolInfo* pi)
{
	static unsigned char preSequence[]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	unsigned char Ack[2];
	unsigned char * Buffer = NULL;
	unsigned char ArgLen;
	ProtocolInfo Info(MAIN_MODULE, SRC_MODULE(pi->Module));
	
	Info.Action = 0xA1;
	Ack[0] = pi->Sequence;
	Ack[1] = pi->Action;
	Info.LinkParameterList(Ack, 2);
	Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
	Info.Send();
	
	if(0x01 == pi->Action)
	{
		ArgLen = strlen(MODULE_INFO);
		Buffer = (unsigned char*)malloc(ArgLen+12);
		strcpy((char*)Buffer, MODULE_INFO);
		memcpy((Buffer+ArgLen), (void*)0x1FFFF7E8,12);
		
		Info.Action = 0x01;
		Info.LinkParameterList(Buffer, ArgLen+12);
		Info.Sequence = ++preSequence[DEST_MODULE(Info.Module)];
		Info.Send();
		free(Buffer);
	}
}
