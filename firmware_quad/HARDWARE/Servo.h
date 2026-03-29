#ifndef __SERVO_H
#define __SERVO_H

#include "main.h"

void HeadMotor(float speed);
void Tail_Motor1(float speed);
void Tail_Motor2(float speed);
void Tail_Motor3(float speed);
void Tail_Motor4(float speed);
void Quad_LeftUpper(float speed);
void Quad_RightUpper(float speed);
void Quad_LeftLower(float speed);
void Quad_RightLower(float speed);
float Calculate_Speed(float speed);

#endif
