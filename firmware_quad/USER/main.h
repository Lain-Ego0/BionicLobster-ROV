#ifndef __MAIN_H__
#define __MAIN_H__

#include "usart.h"
#include "stmflash.h"
#include "led.h"
#include "GPIO.h"
#include "timer.h"
#include "stm32f10x.h"

#include <stdio.h>
#include "stdlib.h"
#include "stdbool.h"
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "stdint.h"

#include "Action.h"

//SYSTEM
#include "delay.h"
#include "sys.h"

//BSP 
#include "IIC.h"

//Hardware
#include "AS5600.h"
#include "MS5837.h"
#include "JY901S.h"
#include "A4988.h"
#include "Motor.h"
#include "Servo.h"

//遥控按键定义
#define Front 0x01
#define Back 0x02
#define Left 0x04
#define Right 0x08
#define Up    0x10
#define Down 0x20
#define Left_Hand_Open 0x01
#define Left_Hand_Close 0x02

#define Right_Hand_Open 0x04
#define Right_Hand_Close 0x08

//#define Left_1Hand_Left 0x01
//#define Left_1Hand_Right 0x02

#define Left_ARM_OPEN 0x10
#define Left_ARM_CLOSE 0x20

#define Right_ARM_OPEN 0x10
#define Right_ARM_CLOSE 0x20

#define Left_Spin  0x04//#define Left_2Hand_Left 0x04
#define Right_Spin 0x08//#define Right_2Hand_Right 0x08

//#define water_push 0x08//#define Left_2Hand_Right 0x08
//#define water_absorption 0x04//#define Right_2Hand_Left 0x04

#define water_push 0x10//#define Left_2Hand_Right 0x08
#define water_absorption 0x20//#define Right_2Hand_Left 0x04

//#define Left_2Hand_Left 0x04
//#define Left_2Hand_Right 0x08

//#define Left_3Hand_Left 0x10
//#define Left_3Hand_Right 0x20

//#define Right_1Hand_Left 0x01
//#define Right_1Hand_Right 0x02

//#define Right_2Hand_Left 0x04
//#define Right_2Hand_Right 0x08

//#define Right_3Hand_Left 0x10
//#define Right_3Hand_Right 0x20

//#define L3   0x10
//#define R3   0x20
//#define L2   0x04
//#define R2   0x08
//#define L1   0x01
//#define R1   0x02

//#define L6   0x10
//#define R6   0x20
//#define L5   0x04
//#define R5   0x08
//#define L4   0x01
//#define R4   0x02

int Get_work_state(void);

	


#endif
