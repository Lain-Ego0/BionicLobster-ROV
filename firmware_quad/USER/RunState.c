#include "RunState.h"
#include <stddef.h>

typedef struct
{
    QuadRunState state;
    uint8_t fault_flags;
} QuadRunStateContext;

static QuadRunStateContext g_quad_run_state;

static uint8_t QuadRunState_IsCommandModeSupported(uint8_t mode)
{
    if (mode == BOARD_CONTROL_MODE_SAFE || mode == BOARD_CONTROL_MODE_STABILIZE)
    {
        return 1u;
    }

    return 0u;
}

static uint8_t QuadRunState_EvaluateFaults(
    const BoardControlCommand *command,
    uint8_t board_link_online,
    uint8_t imu_online)
{
    uint8_t fault_flags = QUAD_FAULT_NONE;

    if (board_link_online == 0u)
    {
        fault_flags |= QUAD_FAULT_BOARD_LINK_LOSS;
    }

    if (command == NULL)
    {
        fault_flags |= QUAD_FAULT_COMMAND_MODE;
        return fault_flags;
    }

    if (QuadRunState_IsCommandModeSupported(command->mode) == 0u)
    {
        fault_flags |= QUAD_FAULT_COMMAND_MODE;
    }

    if (command->mode == BOARD_CONTROL_MODE_STABILIZE && imu_online == 0u)
    {
        fault_flags |= QUAD_FAULT_IMU_OFFLINE;
    }

    return fault_flags;
}

void QuadRunState_Init(void)
{
    g_quad_run_state.state = QUAD_RUN_STATE_SAFE;
    g_quad_run_state.fault_flags = QUAD_FAULT_NONE;
}

void QuadRunState_Update(const BoardControlCommand *command, uint8_t board_link_online, uint8_t imu_online)
{
    uint8_t fault_flags = QuadRunState_EvaluateFaults(command, board_link_online, imu_online);

    if (fault_flags != QUAD_FAULT_NONE)
    {
        g_quad_run_state.state = QUAD_RUN_STATE_FAULT;
        g_quad_run_state.fault_flags = fault_flags;
        return;
    }

    if (g_quad_run_state.state == QUAD_RUN_STATE_FAULT)
    {
        if (command != NULL && command->mode == BOARD_CONTROL_MODE_SAFE)
        {
            g_quad_run_state.state = QUAD_RUN_STATE_SAFE;
            g_quad_run_state.fault_flags = QUAD_FAULT_NONE;
        }
        return;
    }

    g_quad_run_state.fault_flags = QUAD_FAULT_NONE;

    if (command != NULL && command->mode == BOARD_CONTROL_MODE_STABILIZE)
    {
        g_quad_run_state.state = QUAD_RUN_STATE_STABILIZE;
    }
    else
    {
        g_quad_run_state.state = QUAD_RUN_STATE_SAFE;
    }
}

QuadRunState QuadRunState_GetState(void)
{
    return g_quad_run_state.state;
}

uint8_t QuadRunState_GetFaultFlags(void)
{
    return g_quad_run_state.fault_flags;
}
