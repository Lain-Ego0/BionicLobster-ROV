#include "MS5837.h"
/*
C1 压力灵敏度 SENS|T1
C2  压力补偿  OFF|T1
C3	温度压力灵敏度系数 TCS
C4	温度系数的压力补偿 TCO
C5	参考温度 T|REF
C6 	温度系数的温度 TEMP1SENS
*/
//#define MS5837_DATA_SIZE 7			//3+2+2
//#define START_KEY1   0x00
//#define END_KEY1   	 0xFF



uint32_t  Cal_C[7];	        //用于存放PROM中的6组数据1-6************校准系数，C1到C6
double OFF_;
float Aux;
uint32_t  MS5837_date_buffer[MS5837_DATA_SIZE];//用于存放MS5837要发送的数据
uint32_t  MS5837_last_date_buffer[MS5837_DATA_SIZE];//用于存放MS5837上一次发送的数据

/*
dT 实际和参考温度之间的差异
TEMP1 实际温度	
*/
uint64_t dT;
uint64_t TEMP1;
/*
OFF 实际温度补偿
SENS 实际温度灵敏度
*/
uint64_t SENS;
uint32_t D1_Pres,D2_TEMP1;	// 数字压力值,数字温度值
uint32_t TEMP12,T2,OFF2,SENS2;	//温度校验值

uint32_t Pressure;				//气压
uint32_t Atmdsphere_Pressure;//大气压
uint32_t Depth;						//水深
uint32_t TEMP1erature;			//温度

uint32_t last_Depth;
uint32_t change_Depth;


//uint32_t transform_data[5];//帧头、气压、水深、温度

double TEMP1ERATURE=0;
double PRESSURE=0;
double DEPTH=0;
/*******************************************************************************
  * @函数名称	MS583730BA_RESET
  * @函数说明   复位MS5611
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MS583703BA_RESET(void)
{
	MyI2C_Start();
	MyI2C_SendByte(0xEC);//CSB接地，主机地址：0XEE，否则 0X77
	MYIIC_Wait_Ack();
	MyI2C_SendByte(0x1E);//发送复位命令
	MYIIC_Wait_Ack();
	MyI2C_Stop();	
}
/*******************************************************************************
  * @函数名称	MS5611_init
  * @函数说明   初始化5611
  * @输入参数  	无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void MS5837_init(void)
{	 
	uint8_t inth,intl;
	uint8_t i; 
	 
	MyI2C_Init();	         //初始化IIC PB8 PB9口子
	MS583703BA_RESET();	   // Reset Device  复位MS5837
	delay_ms(40);           //复位后延时（注意这个延时是一定必要的，可以缩短但似乎不能少于20ms）
	 
	for (i=1;i<=6;i++) 		//启动
	{
		MyI2C_Start();
		MyI2C_SendByte(0xEC);
		MYIIC_Wait_Ack();
		MyI2C_SendByte(0xA0 + (i*2));
		MYIIC_Wait_Ack();
		MyI2C_Stop();
		delay_ms(5);
		MyI2C_Start();
		MyI2C_SendByte(0xEC+0x01);  //进入接收模式
		delay_ms(1);
		MYIIC_Wait_Ack();
		inth = IIC_Read_Byte(1);  		//带ACK的读数据
		delay_ms(1);
		intl = IIC_Read_Byte(0); 			//最后一个字节NACK
		MyI2C_Stop();
		Cal_C[i] = (((uint16_t)inth << 8) | intl);
	}
	MS5837_Getdata();
}

/**************************实现函数********************************************
*函数原型:unsigned long MS561101BA_getConversion(void)
*功　　能:    读取 MS5837 的转换结果 
*******************************************************************************/
unsigned long MS583703BA_getConversion(uint8_t command)
{
	unsigned long conversion = 0;
	uint8_t TEMP1[3];

	MyI2C_Start();
	MyI2C_SendByte(0xEC); 		//写地址
	MYIIC_Wait_Ack();
	MyI2C_SendByte(command); //写转换命令
	MYIIC_Wait_Ack();
	MyI2C_Stop();

	delay_ms(10);
	MyI2C_Start();
	MyI2C_SendByte(0xEC); 		//写地址
	MYIIC_Wait_Ack();
	MyI2C_SendByte(0);				// start read sequence
	MYIIC_Wait_Ack();
	MyI2C_Stop();

	MyI2C_Start();
	MyI2C_SendByte(0xEC+0x01);  //进入接收模式
	MYIIC_Wait_Ack();
	TEMP1[0] = IIC_Read_Byte(1);  //带ACK的读数据  bit 23-16
	TEMP1[1] = IIC_Read_Byte(1);  //带ACK的读数据  bit 8-15
	TEMP1[2] = IIC_Read_Byte(0);  //带NACK的读数据 bit 0-7
	MyI2C_Stop();

	conversion = (unsigned long)TEMP1[0] * 65536 + (unsigned long)TEMP1[1] * 256 + (unsigned long)TEMP1[2];
	return conversion;
}
///***********************************************
//  * @brief  读取MS5837气压、温度和水深值
//  * @param  None
//  * @retval None
//************************************************
void MS5837_Getdata(void)
{
	static float air_pressure = 985.0f;		// 默认大气压（正常是990-1010之间），保证算出来的是个正值，初始时刻是不是零深度并不重要
	int i=0;
	D1_Pres= MS583703BA_getConversion(0x48);//D1是压力值数据
	D2_TEMP1 = MS583703BA_getConversion(0x58);//D2是温度值数据
	
	if(D2_TEMP1 > (((uint32_t)Cal_C[5])*256))
	{		
		dT=D2_TEMP1 - (((uint32_t)Cal_C[5])*256);
		TEMP1=2000+dT*((uint32_t)Cal_C[6])/8388608;	
		OFF_=(uint32_t)Cal_C[2]*65536+((uint32_t)Cal_C[4]*dT)/128;
		SENS=(uint32_t)Cal_C[1]*32768+((uint32_t)Cal_C[3]*dT)/256;		
	}
	else 
	{	
		dT=(((uint32_t)Cal_C[5])*256) - D2_TEMP1;			
		TEMP1=2000-dT*((uint32_t)Cal_C[6])/8388608;	
		OFF_=(uint32_t)Cal_C[2]*65536-((uint32_t)Cal_C[4]*dT)/128;
		SENS=(uint32_t)Cal_C[1]*32768-((uint32_t)Cal_C[3]*dT)/256;		
	}

	if(TEMP1<2000)  // low TEMP1
	{
	  Aux = (2000-TEMP1)*(2000-TEMP1);	
		T2 = 3*(dT*dT)/8589934592; 
		OFF2 = 3*Aux/2;
		SENS2 = 5*Aux/8;	
	}
	else
	{
	  Aux = (TEMP1-2000)*(TEMP1-2000);		
	  T2 = 2*(dT*dT)/137438953472;
		OFF2 = 1*Aux/16;
		SENS2 = 0;	
	}
	OFF_ = OFF_ - OFF2;
	SENS = SENS - SENS2;	
	TEMP1=(TEMP1-T2)/100;																	//温度
  Pressure= ((D1_Pres*SENS/2097152-OFF_)/8192)/10;		//压力
	Depth=(Pressure-Atmdsphere_Pressure)/0.983615;	 		//水深
	
	for(i=0;i<5;i++)
	{		
		TEMP1ERATURE+=TEMP1;
		PRESSURE+=Pressure;
		DEPTH+=Depth;
	}
	TEMP1ERATURE=TEMP1ERATURE/5;
	PRESSURE=PRESSURE/5;
	DEPTH=(PRESSURE-air_pressure)/ 0.983615;
	
	change_Depth=PRESSURE-last_Depth;
	
	MS5837_date_buffer[0]=START_KEY1;
	MS5837_date_buffer[1]=PRESSURE;
	MS5837_date_buffer[2]=DEPTH;
	MS5837_date_buffer[3]=TEMP1ERATURE;
	MS5837_date_buffer[4]=change_Depth;

	MS5837_date_buffer[6]=END_KEY1;
	
	
}
void sensorReadMS5837(double *TEMP1ERATURE,double *PRESSURE, double *DEPTH)
{
	static float air_pressure = 985.0f;		// 默认大气压（正常是990-1010之间），保证算出来的是个正值，初始时刻是不是零深度并不重要
    //MS5837_Getdata(TEMP1ERATURE,PRESSURE); 		//获取当前气压
		MS5837_Getdata();
	*DEPTH = (*PRESSURE - air_pressure) / 0.983615;
}

//USART6

void Send_MS5837_data(void)
{
//	int i=0;
//	if(USART_GetITStatus(USART6, USART_IT_TXE) != RESET)
//  {   
//		 if (i < 7) // 确保索引不超过数组范围
//        {
//            USART_SendData(USART6, MS5837_date_buffer[i++]); 
//        }

//        if (i == 7) // 全部发送完成
//        {
//            USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
//            i = 0; // 重置索引以便下一次发送
//        }
//		
//    USART_ClearITPendingBit(USART6, USART_IT_TXE); 
//  }
}


void MS5837_last_data_get(void)
{
	int i=0;
	
	for (i = 0; i < 7; i++)
	{
		MS5837_last_date_buffer[i] = MS5837_date_buffer[i];
	}
	last_Depth=MS5837_date_buffer[2];

}


void copy_MS5837_last_data(void) {
    // 复制上一次接收到的数据到数组
    for (int i = 0; i < 7; i++) {
        MS5837_date_buffer[i] = MS5837_last_date_buffer[i];
    }
}
//#define START_KEY1   0x66
//#define START_KEY2   0xb9
//#define END_KEY1   	 0x99
//#define END_KEY2		 0x9b

//void	Send_MS5837_Data(UART_HandleTypeDef *huart,uint64_t* TEMP1,uint32_t* Pressure,uint32_t* Depth)
//{
//	int i=0;
//	int re_falg=0;//更新标志
//	uint8_t date_TEMP1;

//	if(huart==&huart7)
//	{
//		re_falg=1;
//		if(re_falg==1)
//		{
//				MS5837_date_buffer[0]=START_KEY1;
//				MS5837_date_buffer[1]=START_KEY2;
//				MS5837_date_buffer[2]=(uint32_t)*TEMP1;
//				MS5837_date_buffer[3]=*Pressure;
//				MS5837_date_buffer[4]=*Depth;
//				MS5837_date_buffer[5]=END_KEY1;
//				MS5837_date_buffer[6]=END_KEY2;
//			
//				MS5837_last_data_get();
//			
//				for(i=0;i<7;i++)
//				{
//					date_TEMP1=MS5837_date_buffer[i];
//					HAL_UART_Transmit(huart,&date_TEMP1,sizeof(date_TEMP1),10);		
//				}
//			
//		}
//		else
//		{
//			copy_MS5837_last_data();
//			re_falg=1;
//		}
//		
//	
//	
//	}

//}


