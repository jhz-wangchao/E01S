#include<iostream>
#include "COM.h"
#include "Protocol.h"


using namespace std;

ProtocolInfo::ProtocolInfo()
{
	Head = 0xfd;
	Standby1 = 0;
	Lenth = 3;
	Module = 0;
	Sequence = 0;
	ParaLen = 0;
	ParameterList = NULL;
	
}
ProtocolInfo::ProtocolInfo(MODULE Src, MODULE Dst)
{
	Head = 0xfd;
	Standby1 = 0;
	Lenth = 3;
	Module = MODULE_ID(Src, Dst);
	Sequence = 0;
	ParaLen = 0;
	ParameterList = NULL;
}
ProtocolInfo::ProtocolInfo(unsigned char*Fream)
{
	unsigned char * pData = NULL;
	unsigned char DataLen = 0;
	unsigned char i;
	Head = Fream[0];
	Standby1 = Fream[1];
	Lenth = Fream[2];
	Module = Fream[3];
	Sequence = Fream[4];
	Action = Fream[5];
	ParameterList = NULL;
	if(Lenth > 3)
	{
		pData = (unsigned char*)malloc(Lenth - 3);
		for(i=0; i<Lenth-3; i++)
		{
			if(Fream[6+i]==0xfe)
			{
				i++;
				pData[DataLen++] = Fream[6+i] + 0x80;
			}
			else
			{
				pData[DataLen++] = Fream[6+i];
			}
		}
		ParameterList = NULL;
		LinkParameterList(pData, DataLen);
		free(pData);
	}
}
ProtocolInfo::~ProtocolInfo()
{
	if(NULL != ParameterList)
	{
		free(ParameterList);
		ParameterList = NULL;
	}
}

void ProtocolInfo::LinkParameterList(unsigned char* ParameterList, unsigned char ParaLen)
{
	unsigned char i;
	if(NULL != this->ParameterList)
	{
		free(this->ParameterList);
		this->ParameterList = NULL;
	}
	this->ParameterList = (unsigned char*)malloc(ParaLen);
	this->ParaLen = ParaLen;
	for(i=0;i<ParaLen; i++)
	{
		(this->ParameterList)[i] = (ParameterList)[i];
	}
}

int ProtocolInfo::ToArray(unsigned char*Fream)
{
	int lenth;
	int i;
	unsigned char* ptr;
	
	Fream[0] = Head;
	Fream[1] = Standby1;
	Fream[2] = Lenth;
	Fream[3] = Module;
	Fream[4] = Sequence;
	Fream[5] = Action;
	
	ptr = Fream+6;
	for(i=0,lenth=3;i<ParaLen; i++,ptr++,lenth++)
	{
		if((0xfd == ParameterList[i]) || (0xfe == ParameterList[i]) || (0xf8 == ParameterList[i]))
		{
			*(ptr++) = 0xfe;
			*ptr = ParameterList[i] - 0x80;
			lenth++;
		}
		else
		{
			*ptr = ParameterList[i];
		}
	}
	Fream[2] = Lenth = lenth;
	*(ptr++) = Protocol_CalCrc(Fream);
	*(ptr++) = 0xf8;
	return ptr - Fream;
}
void ProtocolInfo::Send()
{
	COM_Number COMx = ComModuleLink[DEST_MODULE(Module)];
	unsigned char * BufferToSend = (unsigned char*)malloc(5 + ParaLen * 2);
	int Lenth = ToArray(BufferToSend);
	COMS[COMx]->Transmits(BufferToSend, Lenth);
}
MODULE ProtocolInfo::getSourceModule()
{
	return SRC_MODULE(Module);
}
MODULE ProtocolInfo::getDestModule()
{
	return DEST_MODULE(Module);
}
void ProtocolInfo::setSourceModule(MODULE Module)
{
	this->Module = MODULE_ID(Module, (MODULE)DEST_MODULE(Module));
}
void ProtocolInfo::setDestModule(MODULE Module)
{
	this->Module = MODULE_ID(SRC_MODULE(Module), Module);
}
void ProtocolInfo::SwapModule()
{
	Module = MODULE_SWEP(Module);
}
