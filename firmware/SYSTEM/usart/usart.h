#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"

#define UART_BUFFER_SIZE 256
#define Start_byte 0xA5
#define End_byte   0x7B
#define CONTROL_FRAME_SIZE 7

extern volatile uint8_t key;
extern volatile uint8_t hand_OC;
extern volatile uint8_t Left_Move;
extern volatile uint8_t Right_Move;
extern volatile uint8_t Water_Control;

void uart_init1(u32 bound);
void uart_init2(u32 bound);
void Serial_SendByte(uint8_t byte);
void BoardLink_SendFrame(void);
void Control_ResetState(void);

#endif


