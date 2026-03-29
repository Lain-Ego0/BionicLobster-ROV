#include "main.h"

/**********************180度舵机角度控制**********************/

// 龙虾左爪
void L_Claw(float Angle) {
    uint16_t Compare = (Angle / 180.0) * 2000 + 500;
    TIM_SetCompare1(TIM1, Compare); // PA8
}
// 龙虾左肘
void L_Elbow(float Angle) {	
    uint16_t Compare = (Angle / 180.0) * 2000 + 500;
    TIM_SetCompare2(TIM1, Compare); // PA9
}
//龙虾右爪
void R_Claw(float Angle) {
    uint16_t Compare = (Angle / 180.0) * 2000 + 500;
    TIM_SetCompare1(TIM2, Compare); // PA0
}
// 龙虾右肘
void R_Elbow(float Angle) {
    uint16_t Compare = (Angle / 180.0) * 2000 + 500;
    TIM_SetCompare2(TIM2, Compare); // PA1
}
// 尾部上舵机
void Tail_Servo1(float speed) {
     uint16_t Compare = Calculate_Speed(speed);
    TIM_SetCompare1(TIM3, Compare); // PA6
}
// 尾部下舵机
void Tail_Servo2(float speed) {
    uint16_t Compare = Calculate_Speed(speed);
    TIM_SetCompare2(TIM3, Compare); // PA7
}



// 函数实现
float Calculate_Speed(float speed) {
    // 确保 speed 在 -5.0 到 5.0 之间
    if (speed < -5.0f) speed = -5.0f;
    if (speed > 5.0f) speed = 5.0f;
    // 中心位置的比较值
    float center = 1490.0f; //中位值是1490
    // 每个速度等级对应的增量
    float increment = 100.0f; //140
    // 根据速度值计算比较值
    float compare = center + (speed * increment);
    // 限幅处理
    if (compare < 800.0f) compare = 800.0f;
    if (compare > 2200.0f) compare = 2200.0f;
    return compare;
}

/**********************360度舵机速度控制**********************/
// 龙虾左手臂上部360
void L_Upper_360(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare3(TIM1, (uint16_t)Compare); // PA10
}
// 龙虾左手臂下部360
void L_Lower_360(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare4(TIM1, (uint16_t)Compare); // PA11
}
// 龙虾右手臂上部360
void R_Upper_360(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare3(TIM2, (uint16_t)Compare); // PB10
}
// 龙虾右手臂下部360
void R_Lower_360(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare4(TIM2, (uint16_t)Compare); // PB11
}

/**********************无刷电机速度控制**********************/
// 左上电机
void L_Motor(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare2(TIM4, (uint16_t)Compare); // PB7
}
// 右上电机
void R_Motor(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare4(TIM3, (uint16_t)Compare); // PB1
}
// 头部电机
void HeadMotor(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare1(TIM4, (uint16_t)Compare); // PB6
}
// 尾部电机3
void Tail_Motor3(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare3(TIM4, (uint16_t)Compare); // PB8
}
// 尾部电机2
void Tail_Motor2(float speed) {
    float Compare = Calculate_Speed(speed);    // 根据速度值计算比较值
    TIM_SetCompare4(TIM4, (uint16_t)Compare); // PB9
}
