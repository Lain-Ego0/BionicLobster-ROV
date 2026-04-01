#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"
#include "control_link.h"
#include "telemetry_link.h"

#define UART_BUFFER_SIZE 256
#define REMOTE_CONTROL_START_BYTE 0xA5u
#define REMOTE_CONTROL_END_BYTE   0x7Bu
#define REMOTE_CONTROL_FRAME_SIZE 7u

extern volatile uint8_t key;
extern volatile uint8_t hand_OC;
extern volatile uint8_t Left_Move;
extern volatile uint8_t Right_Move;
extern volatile uint8_t Water_Control;

void uart_init1(u32 bound);
void uart_init2(u32 bound);
void Serial_SendByte(uint8_t byte);
void BoardLink_SendCommand(const BoardControlCommand *command);
void Control_ResetState(void);
uint8_t QuadTelemetry_IsOnline(void);
void QuadTelemetry_GetStatus(QuadTelemetryStatus *status);
void QuadTelemetry_FlushToHost(void);

#endif


