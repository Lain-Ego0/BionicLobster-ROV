#include "main.h"

static uint16_t Calculate_AnglePulse(float angle)
{
    if (angle < 0.0f)
    {
        angle = 0.0f;
    }
    if (angle > 180.0f)
    {
        angle = 180.0f;
    }

    return (uint16_t)((angle / 180.0f) * 2000.0f + 500.0f);
}

void L_Claw(float angle)
{
    TIM_SetCompare1(TIM1, Calculate_AnglePulse(angle));
}

void L_Elbow(float angle)
{
    TIM_SetCompare2(TIM1, Calculate_AnglePulse(angle));
}

void R_Claw(float angle)
{
    TIM_SetCompare1(TIM2, Calculate_AnglePulse(angle));
}

void R_Elbow(float angle)
{
    TIM_SetCompare2(TIM2, Calculate_AnglePulse(angle));
}

float Calculate_Speed(float speed)
{
    float compare;

    if (speed < -5.0f)
    {
        speed = -5.0f;
    }
    if (speed > 5.0f)
    {
        speed = 5.0f;
    }

    compare = 1500.0f + speed * 100.0f;
    if (compare < 1000.0f)
    {
        compare = 1000.0f;
    }
    if (compare > 2000.0f)
    {
        compare = 2000.0f;
    }

    return compare;
}

void Tail_Servo1(float speed)
{
    TIM_SetCompare1(TIM3, (uint16_t)Calculate_Speed(speed));
}

void Tail_Servo2(float speed)
{
    TIM_SetCompare2(TIM3, (uint16_t)Calculate_Speed(speed));
}

void L_Upper_360(float speed)
{
    TIM_SetCompare3(TIM1, (uint16_t)Calculate_Speed(speed));
}

void L_Lower_360(float speed)
{
    TIM_SetCompare4(TIM1, (uint16_t)Calculate_Speed(speed));
}

void R_Upper_360(float speed)
{
    TIM_SetCompare3(TIM2, (uint16_t)Calculate_Speed(speed));
}

void R_Lower_360(float speed)
{
    TIM_SetCompare4(TIM2, (uint16_t)Calculate_Speed(speed));
}
