#include "main.h"
#include "RunState.h"
#include "Scheduler.h"

typedef struct
{
    BoardControlCommand board_command;
    QuadTelemetryStatus telemetry_status;
    QuadRunState run_state;
    uint8_t board_link_online;
    uint8_t imu_online;
    uint8_t fault_flags;
    uint32_t status_tick_ms;
} QuadRuntimeContext;

static void QuadBoard_RunSupervisorTask(QuadRuntimeContext *runtime)
{
    BoardLink_GetControlCommand(&runtime->board_command);
    runtime->board_link_online = BoardLink_IsOnline();
    runtime->imu_online = JY901S_IsOnline();
    QuadRunState_Update(&runtime->board_command, runtime->board_link_online, runtime->imu_online);
    runtime->run_state = QuadRunState_GetState();
    runtime->fault_flags = QuadRunState_GetFaultFlags();
}

static void QuadBoard_RunTimingTask(void)
{
    BoardLink_Tick();
    JY901S_Tick();
}

static void QuadBoard_RunControlTask(QuadRuntimeContext *runtime)
{
    if (runtime->run_state == QUAD_RUN_STATE_STABILIZE)
    {
        ApplyBoardControlCommand(&runtime->board_command);
    }
    else
    {
        if (runtime->run_state == QUAD_RUN_STATE_FAULT && runtime->board_link_online == 0u)
        {
            Control_ResetState();
        }
        Start();
    }
}

static void QuadBoard_RunStatusTask(QuadRuntimeContext *runtime)
{
    runtime->status_tick_ms = Scheduler_GetTickMs();
    runtime->fault_flags = QuadRunState_GetFaultFlags();
    runtime->telemetry_status.run_state = (uint8_t)runtime->run_state;
    runtime->telemetry_status.fault_flags = runtime->fault_flags;
    runtime->telemetry_status.board_link_online = runtime->board_link_online;
    runtime->telemetry_status.imu_online = runtime->imu_online;
    runtime->telemetry_status.control_mode = runtime->board_command.mode;
    runtime->telemetry_status.surge = runtime->board_command.surge;
    runtime->telemetry_status.yaw = runtime->board_command.yaw;
    runtime->telemetry_status.heave = runtime->board_command.heave;
    TelemetryLink_SendStatus(&runtime->telemetry_status);
}

int main(void)
{
    QuadRuntimeContext runtime;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    MYGPIO_Init();
    PWM_ParameterConfiguration();
    uart_init1(115200);
    uart_init2(115200);
    ESC_Init();
    FlightControl_Init();
    QuadRunState_Init();
    Scheduler_Init();
    Control_ResetState();

    memset(&runtime, 0, sizeof(runtime));
    runtime.run_state = QUAD_RUN_STATE_SAFE;

    while (1)
    {
        uint8_t pending_tasks = Scheduler_TakePendingTasks();

        if (pending_tasks == 0u)
        {
            __WFI();
            continue;
        }

        if ((pending_tasks & QUAD_SCHED_TASK_CONTROL) != 0u)
        {
            QuadBoard_RunTimingTask();
        }

        if ((pending_tasks & QUAD_SCHED_TASK_SUPERVISOR) != 0u)
        {
            QuadBoard_RunSupervisorTask(&runtime);
        }

        if ((pending_tasks & QUAD_SCHED_TASK_STATUS) != 0u)
        {
            QuadBoard_RunStatusTask(&runtime);
        }

        if ((pending_tasks & QUAD_SCHED_TASK_CONTROL) != 0u)
        {
            QuadBoard_RunControlTask(&runtime);
        }
    }
}
