#ifndef __MYI2C_H
#define __MYI2C_H

#include "main.h"


void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);

uint8_t MYIIC_Wait_Ack(void);
uint8_t IIC_Read_Byte(unsigned char ack);
void MYIIC_Ack(void);
void MYIIC_NAck(void);

#endif
