#ifndef __SERVO_H
#define __SERVO_H	

#include "main.h"

// 180度舵机角度控制函数声明
void L_Elbow(float Angle);
void L_Claw(float Angle);
void R_Claw(float Angle);
void R_Elbow(float Angle);


// 360度舵机速度控制函数声明
void L_Upper_360(float speed);
void L_Lower_360(float speed);
void R_Upper_360(float speed);
void R_Lower_360(float speed);
void Tail_Servo1(float speed);
void Tail_Servo2(float speed);

// 无刷电机速度控制函数声明
void L_Motor(float speed);
void R_Motor(float speed);
void HeadMotor(float speed);
void Tail_Motor1(float speed);
void Tail_Motor2(float speed);
void Tail_Motor3(float speed);

// 公共函数声明
float Calculate_Speed(float speed);



#endif


