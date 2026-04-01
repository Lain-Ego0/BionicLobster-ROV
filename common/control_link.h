#ifndef __CONTROL_LINK_H__
#define __CONTROL_LINK_H__

#include <stdint.h>

#define BOARD_CONTROL_AXIS_MIN            (-100)
#define BOARD_CONTROL_AXIS_MAX            100

#define BOARD_CONTROL_MODE_SAFE           0x00u
#define BOARD_CONTROL_MODE_STABILIZE      0x01u

#define BOARD_CONTROL_FLAG_NONE           0x00u

#define BOARD_LINK_START_BYTE             0xA5u
#define BOARD_LINK_END_BYTE               0x7Bu
#define BOARD_LINK_VERSION                0x01u

#define BOARD_LINK_START_INDEX            0u
#define BOARD_LINK_VERSION_INDEX          1u
#define BOARD_LINK_SEQUENCE_INDEX         2u
#define BOARD_LINK_MODE_INDEX             3u
#define BOARD_LINK_SURGE_INDEX            4u
#define BOARD_LINK_YAW_INDEX              5u
#define BOARD_LINK_HEAVE_INDEX            6u
#define BOARD_LINK_FLAGS_INDEX            7u
#define BOARD_LINK_CHECKSUM_INDEX         8u
#define BOARD_LINK_END_INDEX              9u
#define BOARD_LINK_FRAME_SIZE             10u

typedef struct
{
    uint8_t mode;
    int8_t surge;
    int8_t yaw;
    int8_t heave;
    uint8_t flags;
} BoardControlCommand;

static inline int8_t BoardControl_ClampAxis(int value)
{
    if (value < BOARD_CONTROL_AXIS_MIN)
    {
        value = BOARD_CONTROL_AXIS_MIN;
    }
    if (value > BOARD_CONTROL_AXIS_MAX)
    {
        value = BOARD_CONTROL_AXIS_MAX;
    }

    return (int8_t)value;
}

static inline uint8_t BoardLink_CalculateChecksum(const uint8_t *frame)
{
    uint8_t checksum = 0u;
    uint8_t i;

    for (i = BOARD_LINK_VERSION_INDEX; i < BOARD_LINK_CHECKSUM_INDEX; i++)
    {
        checksum ^= frame[i];
    }

    return checksum;
}

#endif
