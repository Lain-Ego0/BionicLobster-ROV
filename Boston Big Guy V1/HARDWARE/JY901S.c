#include "JY901S.h"
#include "string.h"

struct STime		stcTime;
struct SAcc 		stcAcc;
struct SGyro 		stcGyro;
struct SAngle 	stcAngle;
struct SMag 		stcMag;
struct SDStatus stcDStatus;
struct SPress 	stcPress;
struct SLonLat 	stcLonLat;
struct SGPSV 		stcGPSV;
struct SQ       stcQ;

//static unsigned char TxBuffer[256];
//static unsigned char count=0; 


void sendcmd(char *cmd)
{
	char i=0;
	for(i=0;i<5;i++)
	{
		UART2_Put_Char(cmd[i]);
	}
		
}
void UART2_Put_Char(unsigned char DataToSend)
{
//	TxBuffer[count++] = DataToSend;  
//  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  
}

//CopeSerialData为串口2中断调用函数，串口每收到一个数据，调用一次这个函数。
void CopeSerial2Data(unsigned char ucData)
{
//	static unsigned char ucRxBuffer[250];
//	static unsigned char ucRxCnt = 0;	
//	
//	
//	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
//	if (ucRxBuffer[0]!=0x55) //数据头不对，则重新开始寻找0x55数据头
//	{
//		ucRxCnt=0;
//		return;
//	}
//	if (ucRxCnt<11) {return;}//数据不满11个，则返回
//	else
//	{
//		switch(ucRxBuffer[1])//判断数据是哪种数据，然后将其拷贝到对应的结构体中，有些数据包需要通过上位机打开对应的输出后，才能接收到这个数据包的数据
//		{
//			case 0x50:	memcpy(&stcTime,&ucRxBuffer[2],8);break;//memcpy为编译器自带的内存拷贝函数，需引用"string.h"，将接收缓冲区的字符拷贝到数据结构体里面，从而实现数据的解析。
//			case 0x51:	memcpy(&stcAcc,&ucRxBuffer[2],8);break;
//			case 0x52:	memcpy(&stcGyro,&ucRxBuffer[2],8);break;
//			case 0x53:	memcpy(&stcAngle,&ucRxBuffer[2],8);break;
//			case 0x54:	memcpy(&stcMag,&ucRxBuffer[2],8);break;
//			case 0x55:	memcpy(&stcDStatus,&ucRxBuffer[2],8);break;
//			case 0x56:	memcpy(&stcPress,&ucRxBuffer[2],8);break;
//			case 0x57:	memcpy(&stcLonLat,&ucRxBuffer[2],8);break;
//			case 0x58:	memcpy(&stcGPSV,&ucRxBuffer[2],8);break;
//			case 0x59:	memcpy(&stcQ,&ucRxBuffer[2],8);break;
//		}
//		ucRxCnt=0;//清空缓存区}
}

 
