#ifndef __IIC_H__
#define __IIC_H__
#ifdef __cplusplus
extern "C"{
#endif

typedef enum
{
	IIC1,
	IIC2,
	IIC_Total
}IIC_Index;
#ifdef __IO_H__


#define IIC_SCL_PIN_CH1		PinCode(PORTB, 1<<6)
#define IIC_SDA_PIN_CH1		PinCode(PORTB, 1<<7)

#define IIC_SCL_PIN_CH2		PinCode(PORTB, 1<<10)
#define IIC_SDA_PIN_CH2		PinCode(PORTB, 1<<11)

typedef struct
{
	struct
	{
		I2C_HandleTypeDef hI2C;
		IO_HandleTypeDef SCL;
		IO_HandleTypeDef SDA;
	}Channel[IIC_Total];
}IIC_Handle;

#endif

void IIC_Initilize(IIC_Index index);
void IIC_DeInitilize(IIC_Index index);
unsigned char IIC_Transfer(IIC_Index index, unsigned short DevAddress, unsigned char *pData, unsigned short Size, unsigned int Timeout);
unsigned char IIC_Recive(IIC_Index index, unsigned short DevAddress, unsigned char *pData, unsigned short Size, unsigned int Timeout);
unsigned char IIC_Mem_Read(IIC_Index index, unsigned short DevAddress, unsigned short MemAddress, unsigned short MemAddSize, unsigned char *pData, unsigned short Size, unsigned int Timeout);
unsigned char IIC_Mem_Write(IIC_Index index, unsigned short DevAddress, unsigned short MemAddress, unsigned short MemAddSize, unsigned char *pData, unsigned short Size, unsigned int Timeout);
typedef struct _IIC_Driver
{
	void (*Initilize)(void);
	void (*DeInitilize)(void);
	unsigned char (*Tranfer)(unsigned short DevAddress, unsigned char *pData, unsigned short Size, unsigned int Timeout);
	unsigned char (*Recive)(unsigned short DevAddress, unsigned char *pData, unsigned short Size, unsigned int Timeout);
	unsigned char (*MemRead)(unsigned short DevAddress, unsigned short MemAddress, unsigned short MemAddSize, unsigned char *pData, unsigned short Size, unsigned int Timeout);
	unsigned char (*MemWrite)(unsigned short DevAddress, unsigned short MemAddress, unsigned short MemAddSize, unsigned char *pData, unsigned short Size, unsigned int Timeout);
}IIC_Driver, *pIIC_Driver;
extern IIC_Driver IIC1_Driver,IIC2_Driver;
extern pIIC_Driver IIC[];
#define IIC_DRIVER_DEF(A)	\
	void IIC##A##_Initilize(){IIC_Initilize(IIC##A##);} \
	void IIC##A##_DeInitilize(){IIC_DeInitilize(IIC##A##);} \
	unsigned char IIC##A##_Transfer(unsigned short DevAddress, unsigned char *pData, unsigned short Size, unsigned int Timeout)\
	{return IIC_Transfer(IIC##A##, DevAddress, pData, Size, Timeout);} \
	unsigned char IIC##A##_Recive(unsigned short DevAddress, unsigned char *pData, unsigned short Size, unsigned int Timeout)\
	{return IIC_Recive(IIC##A##, DevAddress, pData, Size, Timeout);} \
	unsigned char IIC##A##_Mem_Read(unsigned short DevAddress, unsigned short MemAddress, unsigned short MemAddSize, unsigned char *pData, unsigned short Size, unsigned int Timeout) \
	{return IIC_Mem_Read(IIC##A##, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);} \
	unsigned char IIC##A##_Mem_Write(unsigned short DevAddress, unsigned short MemAddress, unsigned short MemAddSize, unsigned char *pData, unsigned short Size, unsigned int Timeout) \
	{return IIC_Mem_Write(IIC##A##, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);} \
	IIC_Driver IIC##A##Driver = \
	{ \
		IIC##A##_Initilize, \
		IIC##A##_DeInitilize, \
		IIC##A##_Transfer, \
		IIC##A##_Recive, \
		IIC##A##_Mem_Read, \
		IIC##A##_Mem_Write \
	};
#ifdef __cplusplus
}
#endif
#endif
