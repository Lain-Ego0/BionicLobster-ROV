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
#include "Servo.h"

// 遥控按键定义
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

#define Left_ARM_OPEN 0x10
#define Left_ARM_CLOSE 0x20

#define Right_ARM_OPEN 0x10
#define Right_ARM_CLOSE 0x20

#define water_push 0x10
#define water_absorption 0x20

int Get_work_state(void);

#endif
