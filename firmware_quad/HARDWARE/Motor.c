#include "main.h"

void ESC_Init(void)
{
    uint8_t i;

    for (i = 0; i < 100; i++)
    {
        HeadMotor(0.0f);
        Tail_Motor1(0.0f);
        Tail_Motor2(0.0f);
        Tail_Motor3(0.0f);
        Tail_Motor4(0.0f);
        Quad_LeftUpper(0.0f);
        Quad_RightUpper(0.0f);
        Quad_LeftLower(0.0f);
        Quad_RightLower(0.0f);
        delay_ms(20);
    }
}
