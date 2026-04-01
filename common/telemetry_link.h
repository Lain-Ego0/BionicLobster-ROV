#ifndef __TELEMETRY_LINK_H__
#define __TELEMETRY_LINK_H__

#include <stdint.h>

#define TELEMETRY_LINK_START_BYTE            0x5Au
#define TELEMETRY_LINK_END_BYTE              0xA7u
#define TELEMETRY_LINK_VERSION               0x01u

#define TELEMETRY_LINK_START_INDEX           0u
#define TELEMETRY_LINK_VERSION_INDEX         1u
#define TELEMETRY_LINK_SEQUENCE_INDEX        2u
#define TELEMETRY_LINK_RUN_STATE_INDEX       3u
#define TELEMETRY_LINK_FAULT_FLAGS_INDEX     4u
#define TELEMETRY_LINK_BOARD_LINK_INDEX      5u
#define TELEMETRY_LINK_IMU_INDEX             6u
#define TELEMETRY_LINK_MODE_INDEX            7u
#define TELEMETRY_LINK_SURGE_INDEX           8u
#define TELEMETRY_LINK_YAW_INDEX             9u
#define TELEMETRY_LINK_HEAVE_INDEX           10u
#define TELEMETRY_LINK_CHECKSUM_INDEX        11u
#define TELEMETRY_LINK_END_INDEX             12u
#define TELEMETRY_LINK_FRAME_SIZE            13u

typedef struct
{
    uint8_t run_state;
    uint8_t fault_flags;
    uint8_t board_link_online;
    uint8_t imu_online;
    uint8_t control_mode;
    int8_t surge;
    int8_t yaw;
    int8_t heave;
} QuadTelemetryStatus;

static inline uint8_t TelemetryLink_CalculateChecksum(const uint8_t *frame)
{
    uint8_t checksum = 0u;
    uint8_t i;

    for (i = TELEMETRY_LINK_VERSION_INDEX; i < TELEMETRY_LINK_CHECKSUM_INDEX; i++)
    {
        checksum ^= frame[i];
    }

    return checksum;
}

#endif
