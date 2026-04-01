#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"
#include "control_link.h"
#include "telemetry_link.h"

#define UART_BUFFER_SIZE 256
#define BOARD_LINK_TIMEOUT_TICKS 25u

void uart_init1(u32 bound);
void uart_init2(u32 bound);
void Serial_SendByte(uint8_t byte);
void Control_ResetState(void);
void BoardLink_Tick(void);
uint8_t BoardLink_IsOnline(void);
void BoardLink_GetControlCommand(BoardControlCommand *command);
void TelemetryLink_SendStatus(const QuadTelemetryStatus *status);

#endif
