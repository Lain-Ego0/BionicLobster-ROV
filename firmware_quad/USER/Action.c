#include "main.h"

static void RunFlightControl(float surge_speed, float yaw_rate_command_dps, float vertical_thrust)
{
    FlightControlCommand command;

    command.surge_speed = surge_speed;
    command.yaw_rate_command_dps = yaw_rate_command_dps;
    command.vertical_thrust = vertical_thrust;
    FlightControl_Run(&command);
}

static float ScaleBoardAxis(int8_t axis, float max_output)
{
    return ((float)axis * max_output) / (float)BOARD_CONTROL_AXIS_MAX;
}

void Start(void)
{
    FlightControl_Reset();
}

void ApplyBoardControlCommand(const BoardControlCommand *command)
{
    FlightControlCommand flight_command;

    flight_command.surge_speed = 0.0f;
    flight_command.yaw_rate_command_dps = 0.0f;
    flight_command.vertical_thrust = 0.0f;

    if (command != NULL)
    {
        flight_command.surge_speed = ScaleBoardAxis(command->surge, QUAD_TAIL_SPEED_X);
        flight_command.yaw_rate_command_dps = ScaleBoardAxis(command->yaw, QUAD_YAW_RATE_COMMAND_DPS);
        flight_command.vertical_thrust = ScaleBoardAxis(command->heave, QUAD_VERTICAL_THRUST_Z);
    }

    FlightControl_Run(&flight_command);
}

void MoveForward(void)
{
    RunFlightControl(QUAD_TAIL_SPEED_X, 0.0f, 0.0f);
}

void MoveBackward(void)
{
    RunFlightControl(-QUAD_TAIL_SPEED_X, 0.0f, 0.0f);
}

void MoveLeft(void)
{
    RunFlightControl(0.0f, -QUAD_YAW_RATE_COMMAND_DPS, 0.0f);
}

void MoveRight(void)
{
    RunFlightControl(0.0f, QUAD_YAW_RATE_COMMAND_DPS, 0.0f);
}

void TurnLeft(void)
{
    MoveLeft();
}

void TurnRight(void)
{
    MoveRight();
}

void Ascend(void)
{
    RunFlightControl(0.0f, 0.0f, QUAD_VERTICAL_THRUST_Z);
}

void Descend(void)
{
    RunFlightControl(0.0f, 0.0f, -QUAD_VERTICAL_THRUST_Z);
}
