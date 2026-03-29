#ifndef __ms5837_H__
#define __ms5837_H__


#include "sys.h"
#include "IIC.h"

#define MS5837_DATA_SIZE 7			//3+2+2
#define START_KEY1   0x00
#define END_KEY1   	 0xFF


extern double TEMP1ERATURE;
extern double PRESSURE;
extern double DEPTH;

extern uint32_t  MS5837_date_buffer[MS5837_DATA_SIZE];//用于存放MS5837要发送的数据


//void MS5837_init(void);
//void MS5837_Getdata(void);
//void sensorReadMS5837(float *TEMPERATURE,float *PRESSURE, float *DEPTH);
//void sensorReadDepth(float *water_depth);


//extern uint64_t TEMP;
//extern uint32_t Pressure;
//extern uint32_t Depth;

void MS5837_init(void);
void MS5837_Getdata(void);
void MS5837_last_data_get(void);
void copy_MS5837_last_data(void);
//void	Send_MS5837_Data(UART_HandleTypeDef *huart,uint64_t* TEMP,uint32_t* Pressure,uint32_t* Depth);
void sensorReadMS5837(double *TEMPERATURE,double *PRESSURE, double *DEPTH);
void Send_MS5837_data(void);








#endif
