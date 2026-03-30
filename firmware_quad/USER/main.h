#ifndef __MAIN_H__
#define __MAIN_H__

#include "usart.h"
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
#include "FlightControl.h"

// SYSTEM
#include "delay.h"
#include "sys.h"

// BSP
#include "IIC.h"

// Hardware
#include "JY901S.h"
#include "Motor.h"
#include "Servo.h"

// 遥控按键定义
#define Front 0x01
#define Back 0x02
#define Left 0x04
#define Right 0x08
#define Up    0x10
#define Down 0x20

// 从板推进与四旋翼联调参数
#define QUAD_TAIL_SPEED_X             2.0f
#define QUAD_HEAD_SPEED_YAW           2.5f
#define QUAD_YAW_RATE_COMMAND_DPS     35.0f
#define QUAD_VERTICAL_THRUST_Z        2.0f

int Get_work_state(void);

#endif
