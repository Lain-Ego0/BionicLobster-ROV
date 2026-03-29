#include "main.h"

int Get_work_state(void)
{
    if (key == 0x00 && hand_OC == 0x00 && Left_Move == 0x00 && Right_Move == 0x00)
    {
        return 0;
    }

    return 1;
}

volatile uint8_t key = 0x00;
volatile uint8_t hand_OC = 0x00;
volatile uint8_t Left_Move = 0x00;
volatile uint8_t Right_Move = 0x00;
volatile uint8_t Water_Control = 0x00;

static void MainBoard_HandleMotion(void)
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

static void MainBoard_HandleManipulator(void)
{
    if (hand_OC == Left_Hand_Open)
    {
        LeftHand_Open();
    }
    else if (hand_OC == Right_Hand_Open)
    {
        RightHand_Open();
    }

    if (Left_Move == Left_ARM_OPEN)
    {
        LeftARM_Open();
    }

    if (Right_Move == Right_ARM_OPEN)
    {
        RightARM_Open();
    }
}

int workstate=0;

int main(void) 
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置 NVIC 中断分组 2:2 位抢占优先级，2 位响应优先级

    MYGPIO_Init();
    PWM_ParameterConfiguration();
    uart_init1(115200);
    uart_init2(115200);
    MOTOR_Init();
    MS5837_init();
    AS5600_InitAll();
    Control_ResetState();

    while (1)
    {
        AS5600_UpdateAll();
        workstate = Get_work_state();

        if (Left_Move == water_push)
        {
            motor_control(1, 150, 8000);
        }
        else if (Right_Move == water_absorption)
        {
            motor_control(0, 150, 8000);
        }

        if (workstate == 0)
        {
            Start();
        }
        else
        {
            MainBoard_HandleMotion();
            MainBoard_HandleManipulator();
        }

        BoardLink_SendFrame();
        delay_ms(10);
    }
}

