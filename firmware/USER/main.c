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

static void MainBoard_BuildBoardCommand(BoardControlCommand *command)
{
    int surge_input = 0;
    int yaw_input = 0;
    int heave_input = 0;

    command->mode = BOARD_CONTROL_MODE_SAFE;
    command->surge = 0;
    command->yaw = 0;
    command->heave = 0;
    command->flags = BOARD_CONTROL_FLAG_NONE;

    if ((key & Front) != 0u)
    {
        surge_input += BOARD_CONTROL_AXIS_MAX;
    }
    if ((key & Back) != 0u)
    {
        surge_input -= BOARD_CONTROL_AXIS_MAX;
    }
    if ((key & Right) != 0u)
    {
        yaw_input += BOARD_CONTROL_AXIS_MAX;
    }
    if ((key & Left) != 0u)
    {
        yaw_input -= BOARD_CONTROL_AXIS_MAX;
    }
    if ((key & Up) != 0u)
    {
        heave_input += BOARD_CONTROL_AXIS_MAX;
    }
    if ((key & Down) != 0u)
    {
        heave_input -= BOARD_CONTROL_AXIS_MAX;
    }

    command->surge = BoardControl_ClampAxis(surge_input);
    command->yaw = BoardControl_ClampAxis(yaw_input);
    command->heave = BoardControl_ClampAxis(heave_input);

    if (command->surge != 0 || command->yaw != 0 || command->heave != 0)
    {
        command->mode = BOARD_CONTROL_MODE_STABILIZE;
    }
}

static void MainBoard_HandleMotion(void)
{
    if ((key & Front) != 0u && (key & Back) == 0u)
    {
        MoveForward();
    }
    else if ((key & Back) != 0u && (key & Front) == 0u)
    {
        MoveBackward();
    }
    else
    {
        Start();
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
    BoardControlCommand board_command;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置 NVIC 中断分组 2:2 位抢占优先级，2 位响应优先级

    MYGPIO_Init();
    PWM_ParameterConfiguration();
    uart_init1(115200);
    uart_init2(115200);
    MS5837_init();
    AS5600_InitAll();
    Control_ResetState();

    while (1)
    {
        AS5600_UpdateAll();
        workstate = Get_work_state();

        if (workstate == 0)
        {
            Start();
        }
        else
        {
            MainBoard_HandleMotion();
            MainBoard_HandleManipulator();
        }

        MainBoard_BuildBoardCommand(&board_command);
        BoardLink_SendCommand(&board_command);
        QuadTelemetry_FlushToHost();
        delay_ms(10);
    }
}

