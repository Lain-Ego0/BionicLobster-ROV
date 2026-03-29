#include "main.h"

static float ClampSpeed(float value)
{
    if (value < -5.0f)
    {
        value = -5.0f;
    }
    if (value > 5.0f)
    {
        value = 5.0f;
    }
    return value;
}

static void SetTailMotors(float motor1, float motor2, float motor3, float motor4)
{
    Tail_Motor1(motor1);
    Tail_Motor2(motor2);
    Tail_Motor3(motor3);
    Tail_Motor4(motor4);
}

static void ApplyQuadLevelHold(float throttle)
{
    float roll = 0.0f;
    float pitch = 0.0f;
    float roll_correction;
    float pitch_correction;

    if (JY901S_AngleReady() != 0)
    {
        roll = JY901S_GetRollDeg();
        pitch = JY901S_GetPitchDeg();
    }

    roll_correction = ClampSpeed(-roll * QUAD_LEVEL_ROLL_KP);
    pitch_correction = ClampSpeed(-pitch * QUAD_LEVEL_PITCH_KP);

    if (roll_correction < -QUAD_LEVEL_CORRECTION_LIMIT)
    {
        roll_correction = -QUAD_LEVEL_CORRECTION_LIMIT;
    }
    if (roll_correction > QUAD_LEVEL_CORRECTION_LIMIT)
    {
        roll_correction = QUAD_LEVEL_CORRECTION_LIMIT;
    }
    if (pitch_correction < -QUAD_LEVEL_CORRECTION_LIMIT)
    {
        pitch_correction = -QUAD_LEVEL_CORRECTION_LIMIT;
    }
    if (pitch_correction > QUAD_LEVEL_CORRECTION_LIMIT)
    {
        pitch_correction = QUAD_LEVEL_CORRECTION_LIMIT;
    }

    Quad_LeftUpper(ClampSpeed(throttle - pitch_correction - roll_correction));
    Quad_RightUpper(ClampSpeed(throttle - pitch_correction + roll_correction));
    Quad_LeftLower(ClampSpeed(throttle + pitch_correction - roll_correction));
    Quad_RightLower(ClampSpeed(throttle + pitch_correction + roll_correction));
}

void Start(void)
{
    HeadMotor(0.0f);
    SetTailMotors(0.0f, 0.0f, 0.0f, 0.0f);
    ApplyQuadLevelHold(0.0f);
}

void MoveForward(void)
{
    HeadMotor(0.0f);
    SetTailMotors(QUAD_TAIL_SPEED_X, QUAD_TAIL_SPEED_X, QUAD_TAIL_SPEED_X, QUAD_TAIL_SPEED_X);
    ApplyQuadLevelHold(0.0f);
}

void MoveBackward(void)
{
    HeadMotor(0.0f);
    SetTailMotors(-QUAD_TAIL_SPEED_X, -QUAD_TAIL_SPEED_X, -QUAD_TAIL_SPEED_X, -QUAD_TAIL_SPEED_X);
    ApplyQuadLevelHold(0.0f);
}

void MoveLeft(void)
{
    HeadMotor(-QUAD_HEAD_SPEED_YAW);
    SetTailMotors(0.0f, 0.0f, 0.0f, 0.0f);
    ApplyQuadLevelHold(0.0f);
}

void MoveRight(void)
{
    HeadMotor(QUAD_HEAD_SPEED_YAW);
    SetTailMotors(0.0f, 0.0f, 0.0f, 0.0f);
    ApplyQuadLevelHold(0.0f);
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
    HeadMotor(0.0f);
    SetTailMotors(0.0f, 0.0f, 0.0f, 0.0f);
    ApplyQuadLevelHold(QUAD_VERTICAL_THRUST_Z);
}

void Descend(void)
{
    HeadMotor(0.0f);
    SetTailMotors(0.0f, 0.0f, 0.0f, 0.0f);
    ApplyQuadLevelHold(-QUAD_VERTICAL_THRUST_Z);
}
