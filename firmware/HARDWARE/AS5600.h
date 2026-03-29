#ifndef __AS5600_H
#define __AS5600_H

#include "main.h"

#define AS5600_SENSOR_COUNT 5

typedef enum
{
    AS5600_LEFT_ARM_UPPER = 0,
    AS5600_LEFT_ARM_LOWER = 1,
    AS5600_RIGHT_ARM_UPPER = 2,
    AS5600_RIGHT_ARM_LOWER = 3,
    AS5600_TAIL_CABLE_JOINT = 4
} AS5600_SensorId;

typedef struct
{
    uint16_t raw_angle;
    int16_t angle_degree;
    uint16_t angle_fraction;
    uint8_t online;
} AS5600_State;

void AS5600_InitAll(void);
void AS5600_UpdateAll(void);
uint8_t AS5600_ReadRaw(uint8_t sensor_index, uint16_t *raw_angle);
uint8_t AS5600_ReadState(uint8_t sensor_index, AS5600_State *state);
uint8_t AS5600_ReadNamedState(AS5600_SensorId sensor_id, AS5600_State *state);
const AS5600_State *AS5600_GetStates(void);
const AS5600_State *AS5600_GetState(AS5600_SensorId sensor_id);
const char *AS5600_GetSensorName(uint8_t sensor_index);
void AS5600_GetData(int16_t *AS5600_ANGLE,int16_t *angle_val,uint16_t *angle_valL);
void AS5600_Init(void);

#endif
