#include "main.h"

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

void HeadMotor(float speed)
{
    TIM_SetCompare4(TIM3, (uint16_t)Calculate_Speed(speed));
}

void Tail_Motor1(float speed)
{
    TIM_SetCompare1(TIM2, (uint16_t)Calculate_Speed(speed));
}

void Tail_Motor2(float speed)
{
    TIM_SetCompare2(TIM2, (uint16_t)Calculate_Speed(speed));
}

void Tail_Motor3(float speed)
{
    TIM_SetCompare3(TIM2, (uint16_t)Calculate_Speed(speed));
}

void Tail_Motor4(float speed)
{
    TIM_SetCompare4(TIM2, (uint16_t)Calculate_Speed(speed));
}

void Quad_LeftUpper(float speed)
{
    TIM_SetCompare1(TIM1, (uint16_t)Calculate_Speed(speed));
}

void Quad_RightUpper(float speed)
{
    TIM_SetCompare2(TIM1, (uint16_t)Calculate_Speed(speed));
}

void Quad_LeftLower(float speed)
{
    TIM_SetCompare3(TIM1, (uint16_t)Calculate_Speed(speed));
}

void Quad_RightLower(float speed)
{
    TIM_SetCompare4(TIM1, (uint16_t)Calculate_Speed(speed));
}
