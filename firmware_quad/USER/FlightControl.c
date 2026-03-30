#include "main.h"
#include "FlightControl.h"

#define FC_LOOP_DT_S                       0.01f
#define FC_INV_LOOP_DT_S                   100.0f
#define FC_ANGLE_FILTER_ALPHA              0.35f
#define FC_RATE_FILTER_ALPHA               0.30f
#define FC_SURGE_SLEW_STEP                 0.10f
#define FC_VERTICAL_SLEW_STEP              0.08f
#define FC_YAW_RATE_SLEW_STEP_DPS          4.0f
#define FC_ATTITUDE_RATE_LIMIT_DPS         80.0f
#define FC_HEADING_RATE_LIMIT_DPS          45.0f
#define FC_HEADING_HOLD_KP                 1.20f
#define FC_ROLL_ANGLE_KP                   4.0f
#define FC_PITCH_ANGLE_KP                  4.0f
#define FC_ROLL_RATE_KP                    0.055f
#define FC_ROLL_RATE_KI                    0.012f
#define FC_ROLL_RATE_KD                    0.0008f
#define FC_PITCH_RATE_KP                   0.055f
#define FC_PITCH_RATE_KI                   0.012f
#define FC_PITCH_RATE_KD                   0.0008f
#define FC_YAW_RATE_KP                     0.040f
#define FC_YAW_RATE_KI                     0.008f
#define FC_YAW_RATE_KD                     0.0003f
#define FC_RATE_INTEGRAL_LIMIT             0.80f
#define FC_ROLL_OUTPUT_LIMIT               2.0f
#define FC_PITCH_OUTPUT_LIMIT              2.0f
#define FC_YAW_OUTPUT_LIMIT                2.5f
#define FC_YAW_COMMAND_DEADBAND_DPS        1.0f

typedef struct
{
    float kp;
    float ki;
    float kd;
    float integral;
    float previous_measurement;
    float output_limit;
    float integral_limit;
} FlightPid;

typedef struct
{
    uint8_t closed_loop_ready;
    float surge_speed;
    float vertical_thrust;
    float yaw_rate_command_dps;
    float roll_deg;
    float pitch_deg;
    float yaw_deg;
    float roll_rate_dps;
    float pitch_rate_dps;
    float yaw_rate_dps;
    float yaw_hold_deg;
    FlightPid roll_rate_pid;
    FlightPid pitch_rate_pid;
    FlightPid yaw_rate_pid;
} FlightControllerState;

static FlightControllerState g_flight_controller;

static float FlightControl_WrapAngleDeg(float angle);

static float FlightControl_Clamp(float value, float min_value, float max_value)
{
    if (value < min_value)
    {
        value = min_value;
    }
    if (value > max_value)
    {
        value = max_value;
    }
    return value;
}

static float FlightControl_ApplyLowPass(float current, float sample, float alpha)
{
    return current + (sample - current) * alpha;
}

static float FlightControl_ApplyWrappedAngleLowPass(float current, float sample, float alpha)
{
    float delta = FlightControl_WrapAngleDeg(sample - current);

    return FlightControl_WrapAngleDeg(current + delta * alpha);
}

static float FlightControl_ApplySlew(float current, float target, float step)
{
    float delta = target - current;

    if (delta > step)
    {
        delta = step;
    }
    else if (delta < -step)
    {
        delta = -step;
    }

    return current + delta;
}

static float FlightControl_WrapAngleDeg(float angle)
{
    while (angle > 180.0f)
    {
        angle -= 360.0f;
    }

    while (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}

static void FlightPid_Reset(FlightPid *pid, float measurement)
{
    pid->integral = 0.0f;
    pid->previous_measurement = measurement;
}

static float FlightPid_Update(FlightPid *pid, float setpoint, float measurement)
{
    float error = setpoint - measurement;
    float derivative = -(measurement - pid->previous_measurement) * FC_INV_LOOP_DT_S;
    float next_integral = pid->integral + pid->ki * error * FC_LOOP_DT_S;
    float output;
    float clamped_output;

    next_integral = FlightControl_Clamp(next_integral, -pid->integral_limit, pid->integral_limit);
    output = pid->kp * error + next_integral + pid->kd * derivative;
    clamped_output = FlightControl_Clamp(output, -pid->output_limit, pid->output_limit);

    if ((output == clamped_output) ||
        ((clamped_output >= pid->output_limit) && (error < 0.0f)) ||
        ((clamped_output <= -pid->output_limit) && (error > 0.0f)))
    {
        pid->integral = next_integral;
    }

    pid->previous_measurement = measurement;
    return clamped_output;
}

static void FlightControl_ResetLoops(void)
{
    g_flight_controller.closed_loop_ready = 0;
    FlightPid_Reset(&g_flight_controller.roll_rate_pid, 0.0f);
    FlightPid_Reset(&g_flight_controller.pitch_rate_pid, 0.0f);
    FlightPid_Reset(&g_flight_controller.yaw_rate_pid, 0.0f);
}

static void FlightControl_SetTailMotors(float speed)
{
    Tail_Motor1(speed);
    Tail_Motor2(speed);
    Tail_Motor3(speed);
    Tail_Motor4(speed);
}

static void FlightControl_ApplyQuadMix(float vertical, float roll_correction, float pitch_correction)
{
    float left_upper = FlightControl_Clamp(vertical - pitch_correction - roll_correction, -5.0f, 5.0f);
    float right_upper = FlightControl_Clamp(vertical - pitch_correction + roll_correction, -5.0f, 5.0f);
    float left_lower = FlightControl_Clamp(vertical + pitch_correction - roll_correction, -5.0f, 5.0f);
    float right_lower = FlightControl_Clamp(vertical + pitch_correction + roll_correction, -5.0f, 5.0f);

    Quad_LeftUpper(left_upper);
    Quad_RightUpper(right_upper);
    Quad_LeftLower(left_lower);
    Quad_RightLower(right_lower);
}

static void FlightControl_ApplyOpenLoopOutputs(void)
{
    float head_output = 0.0f;

    if (g_flight_controller.yaw_rate_command_dps > FC_YAW_COMMAND_DEADBAND_DPS)
    {
        head_output = QUAD_HEAD_SPEED_YAW;
    }
    else if (g_flight_controller.yaw_rate_command_dps < -FC_YAW_COMMAND_DEADBAND_DPS)
    {
        head_output = -QUAD_HEAD_SPEED_YAW;
    }

    FlightControl_SetTailMotors(g_flight_controller.surge_speed);
    HeadMotor(head_output);
    FlightControl_ApplyQuadMix(g_flight_controller.vertical_thrust, 0.0f, 0.0f);
}

static void FlightControl_UpdateSensors(void)
{
    float roll_deg = JY901S_GetRollDeg();
    float pitch_deg = JY901S_GetPitchDeg();
    float yaw_deg = JY901S_GetYawDeg();
    float roll_rate_dps = JY901S_GetRollRateDegPerSec();
    float pitch_rate_dps = JY901S_GetPitchRateDegPerSec();
    float yaw_rate_dps = JY901S_GetYawRateDegPerSec();

    if (g_flight_controller.closed_loop_ready == 0)
    {
        g_flight_controller.roll_deg = roll_deg;
        g_flight_controller.pitch_deg = pitch_deg;
        g_flight_controller.yaw_deg = yaw_deg;
        g_flight_controller.roll_rate_dps = roll_rate_dps;
        g_flight_controller.pitch_rate_dps = pitch_rate_dps;
        g_flight_controller.yaw_rate_dps = yaw_rate_dps;
        g_flight_controller.yaw_hold_deg = yaw_deg;
        FlightPid_Reset(&g_flight_controller.roll_rate_pid, roll_rate_dps);
        FlightPid_Reset(&g_flight_controller.pitch_rate_pid, pitch_rate_dps);
        FlightPid_Reset(&g_flight_controller.yaw_rate_pid, yaw_rate_dps);
        g_flight_controller.closed_loop_ready = 1;
        return;
    }

    g_flight_controller.roll_deg =
        FlightControl_ApplyLowPass(g_flight_controller.roll_deg, roll_deg, FC_ANGLE_FILTER_ALPHA);
    g_flight_controller.pitch_deg =
        FlightControl_ApplyLowPass(g_flight_controller.pitch_deg, pitch_deg, FC_ANGLE_FILTER_ALPHA);
    g_flight_controller.yaw_deg =
        FlightControl_ApplyWrappedAngleLowPass(g_flight_controller.yaw_deg, yaw_deg, FC_ANGLE_FILTER_ALPHA);
    g_flight_controller.roll_rate_dps =
        FlightControl_ApplyLowPass(g_flight_controller.roll_rate_dps, roll_rate_dps, FC_RATE_FILTER_ALPHA);
    g_flight_controller.pitch_rate_dps =
        FlightControl_ApplyLowPass(g_flight_controller.pitch_rate_dps, pitch_rate_dps, FC_RATE_FILTER_ALPHA);
    g_flight_controller.yaw_rate_dps =
        FlightControl_ApplyLowPass(g_flight_controller.yaw_rate_dps, yaw_rate_dps, FC_RATE_FILTER_ALPHA);
}

void FlightControl_Init(void)
{
    memset(&g_flight_controller, 0, sizeof(g_flight_controller));

    g_flight_controller.roll_rate_pid.kp = FC_ROLL_RATE_KP;
    g_flight_controller.roll_rate_pid.ki = FC_ROLL_RATE_KI;
    g_flight_controller.roll_rate_pid.kd = FC_ROLL_RATE_KD;
    g_flight_controller.roll_rate_pid.output_limit = FC_ROLL_OUTPUT_LIMIT;
    g_flight_controller.roll_rate_pid.integral_limit = FC_RATE_INTEGRAL_LIMIT;

    g_flight_controller.pitch_rate_pid.kp = FC_PITCH_RATE_KP;
    g_flight_controller.pitch_rate_pid.ki = FC_PITCH_RATE_KI;
    g_flight_controller.pitch_rate_pid.kd = FC_PITCH_RATE_KD;
    g_flight_controller.pitch_rate_pid.output_limit = FC_PITCH_OUTPUT_LIMIT;
    g_flight_controller.pitch_rate_pid.integral_limit = FC_RATE_INTEGRAL_LIMIT;

    g_flight_controller.yaw_rate_pid.kp = FC_YAW_RATE_KP;
    g_flight_controller.yaw_rate_pid.ki = FC_YAW_RATE_KI;
    g_flight_controller.yaw_rate_pid.kd = FC_YAW_RATE_KD;
    g_flight_controller.yaw_rate_pid.output_limit = FC_YAW_OUTPUT_LIMIT;
    g_flight_controller.yaw_rate_pid.integral_limit = FC_RATE_INTEGRAL_LIMIT;

    FlightControl_Reset();
}

void FlightControl_Reset(void)
{
    g_flight_controller.surge_speed = 0.0f;
    g_flight_controller.vertical_thrust = 0.0f;
    g_flight_controller.yaw_rate_command_dps = 0.0f;
    FlightControl_ResetLoops();
    FlightControl_SetTailMotors(0.0f);
    HeadMotor(0.0f);
    FlightControl_ApplyQuadMix(0.0f, 0.0f, 0.0f);
}

void FlightControl_Run(const FlightControlCommand *command)
{
    float surge_target = 0.0f;
    float vertical_target = 0.0f;
    float yaw_rate_target = 0.0f;
    float roll_rate_setpoint;
    float pitch_rate_setpoint;
    float yaw_rate_setpoint;
    float yaw_error;
    float roll_correction;
    float pitch_correction;
    float yaw_output;

    if (command != NULL)
    {
        surge_target = FlightControl_Clamp(command->surge_speed, -5.0f, 5.0f);
        vertical_target = FlightControl_Clamp(command->vertical_thrust, -5.0f, 5.0f);
        yaw_rate_target = FlightControl_Clamp(
            command->yaw_rate_command_dps,
            -FC_HEADING_RATE_LIMIT_DPS,
            FC_HEADING_RATE_LIMIT_DPS);
    }

    g_flight_controller.surge_speed =
        FlightControl_ApplySlew(g_flight_controller.surge_speed, surge_target, FC_SURGE_SLEW_STEP);
    g_flight_controller.vertical_thrust =
        FlightControl_ApplySlew(g_flight_controller.vertical_thrust, vertical_target, FC_VERTICAL_SLEW_STEP);
    g_flight_controller.yaw_rate_command_dps =
        FlightControl_ApplySlew(
            g_flight_controller.yaw_rate_command_dps,
            yaw_rate_target,
            FC_YAW_RATE_SLEW_STEP_DPS);

    if (JY901S_IsOnline() == 0)
    {
        FlightControl_ResetLoops();
        FlightControl_ApplyOpenLoopOutputs();
        return;
    }

    FlightControl_UpdateSensors();

    roll_rate_setpoint = FlightControl_Clamp(
        -g_flight_controller.roll_deg * FC_ROLL_ANGLE_KP,
        -FC_ATTITUDE_RATE_LIMIT_DPS,
        FC_ATTITUDE_RATE_LIMIT_DPS);
    pitch_rate_setpoint = FlightControl_Clamp(
        -g_flight_controller.pitch_deg * FC_PITCH_ANGLE_KP,
        -FC_ATTITUDE_RATE_LIMIT_DPS,
        FC_ATTITUDE_RATE_LIMIT_DPS);

    if (fabsf(g_flight_controller.yaw_rate_command_dps) > FC_YAW_COMMAND_DEADBAND_DPS)
    {
        g_flight_controller.yaw_hold_deg = g_flight_controller.yaw_deg;
        yaw_rate_setpoint = g_flight_controller.yaw_rate_command_dps;
    }
    else
    {
        yaw_error = FlightControl_WrapAngleDeg(g_flight_controller.yaw_hold_deg - g_flight_controller.yaw_deg);
        yaw_rate_setpoint = FlightControl_Clamp(
            yaw_error * FC_HEADING_HOLD_KP,
            -FC_HEADING_RATE_LIMIT_DPS,
            FC_HEADING_RATE_LIMIT_DPS);
    }

    roll_correction = FlightPid_Update(
        &g_flight_controller.roll_rate_pid,
        roll_rate_setpoint,
        g_flight_controller.roll_rate_dps);
    pitch_correction = FlightPid_Update(
        &g_flight_controller.pitch_rate_pid,
        pitch_rate_setpoint,
        g_flight_controller.pitch_rate_dps);
    yaw_output = FlightPid_Update(
        &g_flight_controller.yaw_rate_pid,
        yaw_rate_setpoint,
        g_flight_controller.yaw_rate_dps);

    FlightControl_SetTailMotors(g_flight_controller.surge_speed);
    HeadMotor(yaw_output);
    FlightControl_ApplyQuadMix(g_flight_controller.vertical_thrust, roll_correction, pitch_correction);
}
