#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "delay.h"

void LED_Init(void);
void LED_Control(unsigned char LED1,unsigned char LED2,unsigned char LED3,unsigned char LED4);	
void running_water_lamp(unsigned char time_scope);
								
#endif
