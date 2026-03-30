#include "AS5600.h"
#include "IIC.h"

#define AS5600_ADDRESS       0x6C
#define AS5600_ANGLE_H       0x0E
#define AS5600_ANGLE_L       0x0F

typedef struct
{
    SoftI2CBus bus;
    const char *name;
} AS5600_DeviceConfig;

static const AS5600_DeviceConfig g_as5600_devices[AS5600_SENSOR_COUNT] =
{
    {{GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_3}, "left_arm_upper_360"},
    {{GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_12}, "left_arm_lower_360"},
    {{GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_13}, "right_arm_upper_360"},
    {{GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_14}, "right_arm_lower_360"},
    {{GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_15}, "tail_servo1"},
    {{GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_5}, "tail_servo2"},
};

static AS5600_State g_as5600_states[AS5600_SENSOR_COUNT];

static uint8_t AS5600_ReadRegFromBus(const SoftI2CBus *bus, uint8_t reg_address, uint8_t *data)
{
    MyI2C_StartBus(bus);
    MyI2C_SendByteBus(bus, AS5600_ADDRESS);
    if (MyI2C_ReceiveAckBus(bus) != 0)
    {
        MyI2C_StopBus(bus);
        return 0;
    }

    MyI2C_SendByteBus(bus, reg_address);
    if (MyI2C_ReceiveAckBus(bus) != 0)
    {
        MyI2C_StopBus(bus);
        return 0;
    }

    MyI2C_StartBus(bus);
    MyI2C_SendByteBus(bus, AS5600_ADDRESS | 0x01U);
    if (MyI2C_ReceiveAckBus(bus) != 0)
    {
        MyI2C_StopBus(bus);
        return 0;
    }

    *data = MyI2C_ReceiveByteBus(bus);
    MyI2C_SendAckBus(bus, 1);
    MyI2C_StopBus(bus);
    return 1;
}

void AS5600_InitAll(void)
{
    uint8_t i;

    for (i = 0; i < AS5600_SENSOR_COUNT; i++)
    {
        MyI2C_InitBus(&g_as5600_devices[i].bus);
        g_as5600_states[i].raw_angle = 0;
        g_as5600_states[i].angle_degree = 0;
        g_as5600_states[i].angle_fraction = 0;
        g_as5600_states[i].online = 0;
    }
}

void AS5600_Init(void)
{
    AS5600_InitAll();
}

uint8_t AS5600_ReadRaw(uint8_t sensor_index, uint16_t *raw_angle)
{
    uint8_t data_h;
    uint8_t data_l;

    if (sensor_index >= AS5600_SENSOR_COUNT || raw_angle == NULL)
    {
        return 0;
    }

    if (AS5600_ReadRegFromBus(&g_as5600_devices[sensor_index].bus, AS5600_ANGLE_H, &data_h) == 0)
    {
        return 0;
    }

    if (AS5600_ReadRegFromBus(&g_as5600_devices[sensor_index].bus, AS5600_ANGLE_L, &data_l) == 0)
    {
        return 0;
    }

    *raw_angle = (uint16_t)(((uint16_t)data_h << 8) | data_l);
    return 1;
}

uint8_t AS5600_ReadState(uint8_t sensor_index, AS5600_State *state)
{
    uint16_t raw_angle;

    if (state == NULL)
    {
        return 0;
    }

    if (AS5600_ReadRaw(sensor_index, &raw_angle) == 0)
    {
        state->online = 0;
        return 0;
    }

    state->raw_angle = raw_angle;
    state->angle_degree = (int16_t)((360U * raw_angle) / 4096U);
    state->angle_fraction = (uint16_t)((360U * raw_angle) % 4096U);
    state->online = 1;
    return 1;
}

uint8_t AS5600_ReadNamedState(AS5600_SensorId sensor_id, AS5600_State *state)
{
    return AS5600_ReadState((uint8_t)sensor_id, state);
}

void AS5600_UpdateAll(void)
{
    uint8_t i;

    for (i = 0; i < AS5600_SENSOR_COUNT; i++)
    {
        AS5600_ReadState(i, &g_as5600_states[i]);
    }
}

const AS5600_State *AS5600_GetStates(void)
{
    return g_as5600_states;
}

const AS5600_State *AS5600_GetState(AS5600_SensorId sensor_id)
{
    if ((uint8_t)sensor_id >= AS5600_SENSOR_COUNT)
    {
        return NULL;
    }

    return &g_as5600_states[(uint8_t)sensor_id];
}

const char *AS5600_GetSensorName(uint8_t sensor_index)
{
    if (sensor_index >= AS5600_SENSOR_COUNT)
    {
        return "invalid";
    }

    return g_as5600_devices[sensor_index].name;
}

void AS5600_GetData(int16_t *AS5600_ANGLE,int16_t *angle_val,uint16_t *angle_valL)
{
    AS5600_State state;

    if (AS5600_ReadState(0, &state) == 0)
    {
        if (AS5600_ANGLE != NULL)
        {
            *AS5600_ANGLE = 0;
        }
        if (angle_val != NULL)
        {
            *angle_val = 0;
        }
        if (angle_valL != NULL)
        {
            *angle_valL = 0;
        }
        return;
    }

    if (AS5600_ANGLE != NULL)
    {
        *AS5600_ANGLE = (int16_t)state.raw_angle;
    }
    if (angle_val != NULL)
    {
        *angle_val = state.angle_degree;
    }
    if (angle_valL != NULL)
    {
        *angle_valL = state.angle_fraction;
    }
}
