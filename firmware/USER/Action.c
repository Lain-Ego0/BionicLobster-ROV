#include "main.h"

static void ResetHandPose(void)
{
    L_Elbow(35.0f);
    L_Claw(0.0f);
    R_Claw(0.0f);
    R_Elbow(35.0f);
}

static void StopContinuousServos(void)
{
    L_Upper_360(0.0f);
    L_Lower_360(0.0f);
    R_Upper_360(0.0f);
    R_Lower_360(0.0f);
    Tail_Servo1(0.0f);
    Tail_Servo2(0.0f);
}

void Start(void)
{
    ResetHandPose();
    StopContinuousServos();
}

void MoveForward(void)
{
    ResetHandPose();
    L_Upper_360(0.0f);
    L_Lower_360(0.0f);
    R_Upper_360(0.0f);
    R_Lower_360(0.0f);
    Tail_Servo1(-1.0f);
    Tail_Servo2(2.0f);
}

void MoveBackward(void)
{
    ResetHandPose();
    L_Upper_360(0.0f);
    L_Lower_360(0.0f);
    R_Upper_360(0.0f);
    R_Lower_360(0.0f);
    Tail_Servo1(-1.0f);
    Tail_Servo2(2.0f);
}

void MoveLeft(void)
{
    Start();
}

void MoveRight(void)
{
    Start();
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
    Start();
}

void Descend(void)
{
    Start();
}

void LeftHand_Open(void)
{
    ResetHandPose();
    L_Claw(135.0f);
    L_Upper_360(-1.0f);
    L_Lower_360(1.0f);
    R_Upper_360(0.0f);
    R_Lower_360(0.0f);
    Tail_Servo1(0.0f);
    Tail_Servo2(0.0f);
}

void RightHand_Open(void)
{
    ResetHandPose();
    R_Claw(135.0f);
    L_Upper_360(0.0f);
    L_Lower_360(0.0f);
    R_Upper_360(1.0f);
    R_Lower_360(-1.0f);
    Tail_Servo1(0.0f);
    Tail_Servo2(0.0f);
}

void LeftARM_Open(void)
{
    Start();
    L_Elbow(100.0f);
}

void RightARM_Open(void)
{
    Start();
    R_Elbow(100.0f);
}

void LeftHandGrab(void)
{
    LeftHand_Open();
}

void RightHandGrab(void)
{
    RightHand_Open();
}
