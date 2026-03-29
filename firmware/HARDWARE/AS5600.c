#include "AS5600.h"
#include "IIC.h"

#define AS5600_ADDRESS		0x6C		//AS5600的I2C从机地址
#define AS5600_ANGLE_H		0x0E
#define AS5600_ANGLE_L		0x0F


/**
  * 函    数：AS5600读寄存器
  * 参    数：RegAddress 寄存器地址，范围：参考AS5600手册的寄存器描述
  * 返 回 值：读取寄存器的数据，范围：0x00~0xFF
  */
uint8_t AS5600_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();						//I2C起始
	MyI2C_SendByte(AS5600_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
	MyI2C_ReceiveAck();					//接收应答
	MyI2C_SendByte(RegAddress);			//发送寄存器地址
	MyI2C_ReceiveAck();					//接收应答
	
	MyI2C_Start();						//I2C重复起始
	MyI2C_SendByte(AS5600_ADDRESS | 0x01);	//发送从机地址，读写位为1，表示即将读取
	MyI2C_ReceiveAck();					//接收应答
	Data = MyI2C_ReceiveByte();			//接收指定寄存器的数据
	MyI2C_SendAck(1);					//发送应答，给从机非应答，终止从机的数据输出
	MyI2C_Stop();						//I2C终止	
	return Data;
}

/**
  * 函    数：AS5600初始化
  * 参    数：无
  * 返 回 值：无
  */
void AS5600_Init(void)
{
	MyI2C_Init();									//先初始化底层的I2C
	
}


/**
  * 函    数：AS5600获取数据
  * 参    数：
  * 参    数：
  * 返 回 值：无
  */
void AS5600_GetData(int16_t *AS5600_ANGLE,int16_t *angle_val,uint16_t *angle_valL)
{
	uint8_t DataH, DataL;								//定义数据高8位和低8位的变量
	
	DataH = AS5600_ReadReg(AS5600_ANGLE_H);		//读取加速度计X轴的高8位数据
	DataL = AS5600_ReadReg(AS5600_ANGLE_L);		//读取加速度计X轴的低8位数据
	*AS5600_ANGLE = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	*angle_val = 360*(*AS5600_ANGLE)/4096;
	*angle_valL =360*(*AS5600_ANGLE)%4096;
}



