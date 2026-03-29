//为标准500 1000 1500 2000 2500

//TIM1CH 1234
/*PA8 PA9 PA10 PA11*/
/*
PA8		龙虾左爪
PA9 	龙虾左肘
PA10 	龙虾左手臂上部360
PA11	龙虾左手臂下部360
*/

//TIM2CH 1234
/*PA0 PA1 PB10 PB11*/
/*
PA0		龙虾右爪
PA1 	龙虾右肘
PB10 	龙虾右手臂上部360
PB11	龙虾右手臂下部360
*/

//TIM3CH 124
/*PA6 PA7  PB1*/
//TIM4CH 2
/*PB7*/
/*
PA6		尾部上舵机
PA7		尾部下舵机
PB7		左上电机 
PB1		右上电机
*/

//TIM4CH 1234
/*PB6 PB7 PB8 PB9*/
/*
PB6 	头部电机
PB7 	尾部电机1
PB8 	尾部电机3
PB9		尾部电机2
*/

/*
// 180度舵机角度控制函数声明
void L_Elbow(float Angle);手臂
void L_Claw(float Angle);爪子
void R_Claw(float Angle);
void R_Elbow(float Angle);
void Tail_Servo1(float Angle);//尾部上舵机
void Tail_Servo2(float Angle);

// 360度舵机速度控制函数声明
void L_Upper_360(int speed);
void L_Lower_360(int speed);
void R_Upper_360(int speed);
void R_Lower_360(int speed);

// 无刷电机速度控制函数声明
void L_Motor(int speed);
void R_Motor(int speed);
void HeadMotor(int speed);
void Tail_Motor1(int speed);
void Tail_Motor2(int speed);
void Tail_Motor3(int speed);
*/