#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define UART_BUFFER_SIZE 256
#define Start_byte 0xA5
#define End_byte   0x7B

extern uint8_t key;
extern uint8_t hand_OC;//打开关闭
extern uint8_t Left_Move;
extern uint8_t Right_Move;
extern uint8_t Water_Control;


void uart_init2(u32 bound);
void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

void reset_receive_state(void);
void copy_last_data(void);
void process_data(void);

#endif


