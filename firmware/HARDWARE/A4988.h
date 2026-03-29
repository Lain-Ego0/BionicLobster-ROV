#ifndef _A4988_H__
#define _A4988_H__

#include "delay.h"
void MOTOR_Init(void);
void motor_control(unsigned int motor_dir,unsigned int num,unsigned int speed);


#endif

