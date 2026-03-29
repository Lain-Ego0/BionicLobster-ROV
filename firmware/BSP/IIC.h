#ifndef __MYI2C_H
#define __MYI2C_H

#include "stm32f10x.h"
#include <stdint.h>

typedef struct
{
    GPIO_TypeDef *scl_port;
    uint16_t scl_pin;
    GPIO_TypeDef *sda_port;
    uint16_t sda_pin;
} SoftI2CBus;

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

void MyI2C_InitBus(const SoftI2CBus *bus);
void MyI2C_StartBus(const SoftI2CBus *bus);
void MyI2C_StopBus(const SoftI2CBus *bus);
void MyI2C_SendByteBus(const SoftI2CBus *bus, uint8_t byte);
uint8_t MyI2C_ReceiveByteBus(const SoftI2CBus *bus);
void MyI2C_SendAckBus(const SoftI2CBus *bus, uint8_t ack_bit);
uint8_t MyI2C_ReceiveAckBus(const SoftI2CBus *bus);
uint8_t MYIIC_Wait_AckBus(const SoftI2CBus *bus);
uint8_t IIC_Read_ByteBus(const SoftI2CBus *bus, unsigned char ack);
void MYIIC_AckBus(const SoftI2CBus *bus);
void MYIIC_NAckBus(const SoftI2CBus *bus);

#endif
