#include<iostream>
#include "COM.h"
#include "Protocol.h"
#include "tinyos.h"
using namespace std;
const COM_Number ComModuleLink[] = { 
	COM_TOTAL,
	COM2,			//PAD_MODULE	 = 0x01,
	COM_TOTAL,		//MAIN_MODULE = 0x02, 
	COM1,			//ULT_MODULE	 = 0x03,
	COM_TOTAL,		//MOTOR_MODULE= 0x04,
	COM3,			//INFRARED_MODULE = 0x05,
	COM_TOTAL,		//PROJECTION_MODULE = 0x06,
	COM_TOTAL,		//CHARGING_MODULE = 0x07,
	COM_TOTAL,		//FACEMOT_MODULE = 0x08,
	COM_TOTAL,		//HEADWING_MODULE = 0x09,
	COM_TOTAL,		//SLIDING_MODULE = 0x0A,
	COM_TOTAL,		//LIGHT_MODULE = 0x0B,
	COM_TOTAL,		//POWER_MODULE = 0x0C,
	COM_TOTAL,		//WAREHOUSE_MODULE = 0x0D, 
};

static Protocol_DealFunType const Protocol_DealFun[] = {
	Protocol_DealFun_Null,
	Protocol_DealFun_Null,			//PAD_MODULE	 = 0x01,
	Protocol_DealFun_Main,			//MAIN_MODULE = 0x02, 
	Protocol_DealFun_Null,			//ULT_MODULE	 = 0x03,
	Protocol_DealFun_Null,			//MOTOR_MODULE= 0x04,
	Protocol_DealFun_Null,			//INFRARED_MODULE = 0x05,
	Protocol_DealFun_Projector,			//PROJECTION_MODULE = 0x06,
	Protocol_DealFun_Null,			//CHARGING_MODULE = 0x07,
	Protocol_DealFun_Null,			//FACEMOT_MODULE = 0x08,
	Protocol_DealFun_Heading,		//HEADWING_MODULE = 0x09,
	Protocol_DealFun_Sliding,		//SLIDING_MODULE = 0x0A,
	Protocol_DealFun_Light,			//LIGHT_MODULE = 0x0B,
	Protocol_DealFun_Power,			//POWER_MODULE = 0x0C,
	Protocol_DealFun_WareHouse,		//WAREHOUSE_MODULE = 0x0D, 
};
static Protocol_UploadFunType const Protocol_UploadFun[] = {
	Protocol_UploadFun_Null,
	Protocol_UploadFun_Null,		//PAD_MODULE	 = 0x01,
	Protocol_UploadFun_Null,		//MAIN_MODULE = 0x02, 
	Protocol_UploadFun_Null,		//ULT_MODULE	 = 0x03,
	Protocol_UploadFun_Null,		//MOTOR_MODULE= 0x04,
	Protocol_UploadFun_Null,		//INFRARED_MODULE = 0x05,
	Protocol_UploadFun_Null,		//PROJECTION_MODULE = 0x06,
	Protocol_UploadFun_Null,		//CHARGING_MODULE = 0x07,
	Protocol_UploadFun_Null,		//FACEMOT_MODULE = 0x08,
	Protocol_UploadFun_Heading,		//HEADWING_MODULE = 0x09,
	Protocol_UploadFun_Null,		//SLIDING_MODULE = 0x0A,
	Protocol_UploadFun_Null,		//LIGHT_MODULE = 0x0B,
	Protocol_UploadFun_Null,		//POWER_MODULE = 0x0C,
	Protocol_UploadFun_WareHouse,	//WAREHOUSE_MODULE = 0x0D, 
};
ProtocolStaticArgs::ProtocolStaticArgs(MODULE SourceModule)
{
	this->ComIndex = ComModuleLink[SourceModule];

	this->SourceModule = SourceModule;
	this->StartFlag = 0xff;
	this->Index = this->HeadIndex = 0;
	this->Buffer = NULL;
}
void ProtocolStaticArgs::InitProtocolCom(void)
{
	COMS[ComIndex]->Initilize();
	COMS[ComIndex]->SetFreeFlag(1);
	COMS[ComIndex]->SetEnable(1);
}
unsigned long Protocol_Reciving(void* args)
{
	static unsigned char preSequence[]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	ProtocolStaticArgs *Args = (ProtocolStaticArgs*)args;
	
	unsigned char data;
	ProtocolInfo * pi;
	while(COMS[Args->ComIndex]->GetRecivedDataSize() > 0)
	{
		data = COMS[Args->ComIndex]->GetRecivedData();
		if(0xfd == data)
		{
			Args->HeadIndex = 0;
			Args->Index = 0;
			Args->StartFlag = 0x00;
			Args->Head[Args->HeadIndex++] = data;
			if(Args->Buffer != NULL)
			{
				free(Args->Buffer);
				Args->Buffer = NULL;
			}
			continue;
		}
		if(0xF8 == data)
		{
			if(Args->Buffer)
			{
				Args->Buffer[Args->Index++] = data;
				if(Protocol_Check(Args->Buffer))
				{
					pi = new ProtocolInfo(Args->Buffer);
					if(preSequence[SRC_MODULE(pi->Module)] != pi->Sequence)
					{
						preSequence[SRC_MODULE(pi->Module)] = pi->Sequence;
						if(SRC_MODULE(pi->Module) == Args->SourceModule)
							Task_Trig("Protocol", pi, 0);
					}
				}
				free(Args->Buffer);
				Args->Buffer = NULL;
			}
			Args->StartFlag = 0xff;
			continue;
		}
		switch(Args->StartFlag)
		{
			case 0xff:
				break;
			case 0x00:
				Args->Head[Args->HeadIndex++] = data;
				if(3 == Args->HeadIndex)
				{
					if(Args->Buffer != NULL)
					{
						free(Args->Buffer);
						Args->Buffer = NULL;
					}
					Args->Buffer = (unsigned char*)malloc(data + 5);
					for(Args->Index = 0; Args->Index<3; Args->Index++)
					{
						Args->Buffer[Args->Index] = Args->Head[Args->Index];
					}
					Args->StartFlag = 0x01;
				}
				break;
			case 0x01:
				Args->Buffer[Args->Index++] = data;
				if(Args->Index == Args->Buffer[2] + 5)
				{
					free(Args->Buffer);
					Args->Buffer = NULL;
					Args->StartFlag = 0xff;
				}
				break;
			default:
				break;
		}
	}
	return 0;
}
unsigned long Protocol_Deal(void* args)
{
	ProtocolInfo * pi = (ProtocolInfo*) args;
	
	if(ComModuleLink[pi->getDestModule()] < COM_TOTAL)
	{
		pi->Send();
	}
	Protocol_DealFun[pi->getDestModule()](pi);
	
	delete pi;
	return 0;
}
unsigned long Protocol_Up(void* args)
{
	for(int m=PAD_MODULE; m<=WAREHOUSE_MODULE; m++)
	{
		Protocol_UploadFun[m]();
	}
	return 0;
}
unsigned char Protocol_CalCrc(unsigned char *Fream)
{
	unsigned char sum = 0;
	int i;
	
	for(i=2; i<Fream[2] + 3; i++)
	{
		sum += Fream[i];
	}
	return sum;
}
bool Protocol_Check(unsigned char *Fream)
{
	if(0xFD == Fream[0] &&
		0xF8 == Fream[Fream[2] + 4] &&
		Protocol_CalCrc(Fream) == Fream[Fream[2] + 3])
	{
		return true;
	}
	return false;
}
__weak void Protocol_DealFun_Null(ProtocolInfo* pi){(void)pi;}
__weak void Protocol_DealFun_Main(ProtocolInfo* pi){(void)pi;}
__weak void Protocol_DealFun_Heading(ProtocolInfo* pi){(void)pi;}
__weak void Protocol_DealFun_Projector(ProtocolInfo* pi){(void)pi;}
__weak void Protocol_DealFun_Sliding(ProtocolInfo* pi){(void)pi;}
__weak void Protocol_DealFun_Light(ProtocolInfo* pi){(void)pi;}
__weak void Protocol_DealFun_Power(ProtocolInfo* pi){(void)pi;}
__weak void Protocol_DealFun_WareHouse(ProtocolInfo* pi){(void)pi;}
__weak void Protocol_UploadFun_Null(void){}
__weak void Protocol_UploadFun_Main(void){}
__weak void Protocol_UploadFun_Heading(void){}
__weak void Protocol_UploadFun_WareHouse(void){}
