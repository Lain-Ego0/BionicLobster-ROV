#ifndef __RUN_STATE_H__
#define __RUN_STATE_H__

#include "control_link.h"
#include <stdint.h>

typedef enum
{
    QUAD_RUN_STATE_SAFE = 0,
    QUAD_RUN_STATE_STABILIZE = 1,
    QUAD_RUN_STATE_FAULT = 2
} QuadRunState;

#define QUAD_FAULT_NONE                0x00u
#define QUAD_FAULT_BOARD_LINK_LOSS     0x01u
#define QUAD_FAULT_IMU_OFFLINE         0x02u
#define QUAD_FAULT_COMMAND_MODE        0x04u

void QuadRunState_Init(void);
void QuadRunState_Update(const BoardControlCommand *command, uint8_t board_link_online, uint8_t imu_online);
QuadRunState QuadRunState_GetState(void);
uint8_t QuadRunState_GetFaultFlags(void);

#endif
