#ifndef __FLIGHT_CONTROL_H__
#define __FLIGHT_CONTROL_H__

typedef struct
{
    float surge_speed;
    float yaw_rate_command_dps;
    float vertical_thrust;
} FlightControlCommand;

void FlightControl_Init(void);
void FlightControl_Reset(void);
void FlightControl_Run(const FlightControlCommand *command);

#endif
