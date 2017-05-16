#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
#ifdef __cplusplus


typedef enum{ 
	PAD_MODULE	 = 0x01,
	MAIN_MODULE = 0x02, 
	ULT_MODULE	 = 0x03,
	MOTOR_MODULE= 0x04,
	INFRARED_MODULE = 0x05,
	PROJECTION_MODULE = 0x06,
	CHARGING_MODULE = 0x07,
	FACEMOT_MODULE = 0x08,
	HEADWING_MODULE = 0x09,
	SLIDING_MODULE = 0x0A,
	LIGHT_MODULE = 0x0B,
	POWER_MODULE = 0x0C,
	WAREHOUSE_MODULE = 0x0D, 
}MODULE;

#define DEST_MODULE(MODULEID)		((MODULE)((MODULEID)&0x0f))
#define SRC_MODULE(MODULEID)		((MODULE)(((MODULEID)>>4)&0x0f))
#define MODULE_ID(SRC, DEST)		((((unsigned char)((SRC)&0x0f))<<4)|((DEST)&0x0f))
#define MODULE_SWEP(ID)				MODULE_ID(DEST_MODULE(ID), SRC_MODULE(ID))

class ProtocolInfo
{
public:
	unsigned char Head;		//帧头
	unsigned char Standby1;	//预留
	unsigned char Lenth;	//帧长
	unsigned char Module; //目标模块
	unsigned char Sequence;	//序号
	unsigned char Action;	//指令码 
	unsigned char* ParameterList;	//参数
	unsigned char CheckSum;//校验和 
	unsigned char Tail;		//帧尾 
	unsigned char ParaLen;	//参数长度
	unsigned char AllLen;	//总长度

	ProtocolInfo();
	ProtocolInfo(unsigned char*Fream);
	ProtocolInfo(MODULE Src, MODULE Dst);
	int ToArray(unsigned char*Fream);
	~ProtocolInfo();
	void LinkParameterList(unsigned char* ParameterList, unsigned char ParaLen);
	void Send();
	MODULE getSourceModule();
	MODULE getDestModule();
	void setSourceModule(MODULE Module);
	void setDestModule(MODULE Module);
	void SwapModule();
};
unsigned long Protocol_Deal(void* args);
unsigned long Protocol_Reciving(void* args);
unsigned long Protocol_Up(void* args);
//这个结构体储存协议解析函数所需的静态变量，由于存在多个协议解析进程，所以不能使用静态变量
class ProtocolStaticArgs
{
private:
	int ComIndex;
	MODULE SourceModule;
	unsigned char StartFlag;
	unsigned char Head[3];
	unsigned char HeadIndex, Index;
	unsigned char *Buffer;
	friend unsigned long Protocol_Reciving(void* args);
public:
	//构造函数：SourceModule，此协议栈所处理的模块编号
	ProtocolStaticArgs(MODULE SourceModule);
	void InitProtocolCom(void);
};
extern "C"
{
	typedef void (*Protocol_DealFunType)(ProtocolInfo* pi);
	typedef void (*Protocol_UploadFunType)(void);
	#ifdef __COMM_H__
	extern const COM_Number ComModuleLink[];
	#endif
	
	unsigned long Projector_Reciving(void* args);
	
	unsigned char Protocol_CalCrc(unsigned char * Fream);
	bool Protocol_Check(unsigned char * Fream);
	void Protocol_DealFun_Null(ProtocolInfo* pi);
	void Protocol_DealFun_Main(ProtocolInfo* pi);
	void Protocol_DealFun_Heading(ProtocolInfo* pi);
	void Protocol_DealFun_Projector(ProtocolInfo* pi);
	void Protocol_DealFun_Sliding(ProtocolInfo* pi);
	void Protocol_DealFun_Light(ProtocolInfo* pi);
	void Protocol_DealFun_Power(ProtocolInfo* pi);
	void Protocol_DealFun_WareHouse(ProtocolInfo* pi);
	
	void Protocol_UploadFun_Null(void);
	void Protocol_UploadFun_Main(void);
	void Protocol_UploadFun_Heading(void);
	void Protocol_UploadFun_Heading(void);
	void Protocol_UploadFun_WareHouse(void);
}
#endif
#endif
