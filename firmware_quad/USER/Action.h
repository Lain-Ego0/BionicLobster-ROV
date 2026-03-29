#ifndef __ACTION_H__
#define __ACTION_H__


// 函数声明
void Start(void);

// 前进
void MoveForward(void);

// 后退
void MoveBackward(void);

// 左移
void MoveLeft(void);

// 右移
void MoveRight(void);

// 左转
void TurnLeft(void);

// 右转
void TurnRight(void);

// 上升
void Ascend(void);

// 下降
void Descend(void);

// 一键左手抓夹
void LeftHandGrab(void);

// 一键右手抓夹
void RightHandGrab(void);

//左臂舒展
void LeftARM_Open(void) ;

//右臂舒展
void RightARM_Open(void) ;

//一键左夹爪打开(按下为关，松开为放)
void LeftHand_Open(void) ;

//一键右夹爪打开(按下为关，松开为放)
void RightHand_Open(void);




#endif

