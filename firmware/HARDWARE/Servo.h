#ifndef __SERVO_H
#define __SERVO_H	

#include "main.h"

void L_Elbow(float Angle);
void L_Claw(float Angle);
void R_Claw(float Angle);
void R_Elbow(float Angle);

void L_Upper_360(float speed);
void L_Lower_360(float speed);
void R_Upper_360(float speed);
void R_Lower_360(float speed);
void Tail_Servo1(float speed);
void Tail_Servo2(float speed);

float Calculate_Speed(float speed);

#endif


