#include "main.h"

volatile uint8_t key = 0x00;
volatile uint8_t hand_OC = 0x00;
volatile uint8_t Left_Move = 0x00;
volatile uint8_t Right_Move = 0x00;
volatile uint8_t Water_Control = 0x00;

int Get_work_state(void)
{
    if (key == 0x00)
    {
        return 0;
    }

    return 1;
}

static void QuadBoard_HandleMotion(void)
{
    switch (key)
    {
        case Front:
            MoveForward();
            break;
        case Back:
            MoveBackward();
            break;
        case Left:
            MoveLeft();
            break;
        case Right:
            MoveRight();
            break;
        case Up:
            Ascend();
            break;
        case Down:
            Descend();
            break;
        default:
            Start();
            break;
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    MYGPIO_Init();
    PWM_ParameterConfiguration();
    uart_init1(115200);
    uart_init2(115200);
    ESC_Init();
    FlightControl_Init();
    Control_ResetState();

    while (1)
    {
        BoardLink_Tick();
        JY901S_Tick();

        if (BoardLink_IsOnline() == 0)
        {
            Control_ResetState();
            Start();
        }
        else if (Get_work_state() == 0)
        {
            Start();
        }
        else
        {
            QuadBoard_HandleMotion();
        }

        delay_ms(10);
    }
}
