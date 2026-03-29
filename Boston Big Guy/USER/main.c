#include "main.h"

int Get_work_state(void)
{
	//如果没有按键按下
	if(key==0x00&&hand_OC==0x00&&Left_Move==0x00&&Right_Move==0x00)
	{
	//	Keep_balance();
		return 0;
	}
	else			//运动
	{
	//	Keep_balance();
		return 1;
	}
}
/*
3l  temp2 0x10
3r  temp2 0x20
2l  temp2 0x04
2r  temp2 0x08
1l  temp2 0x01
1r  temp2 0x02

6l  temp3 0x10
6r  temp3 0x20
5l  temp3 0x04
5r  temp3 0x08
4l  temp3 0x01
4r  temp3 0x02
*/

//指令的定义
uint8_t key=0x00;
uint8_t hand_OC=0x00;//打开关闭
uint8_t Left_Move=0x00;
uint8_t Right_Move=0x00;
uint8_t Water_Control=0x00;

int workstate=0;
int test=1500;

int main(void) 
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置 NVIC 中断分组 2:2 位抢占优先级，2 位响应优先级

    // 主要初始化
    MYGPIO_Init();            // PWM 的 IO 初始化
    PWM_ParameterConfiguration();    // PWM 定时器初始化
    uart_init2(115200);       // 串口初始化为 115200
//  LED_Init();
    ESC_Init();               // 电调初始化
	MOTOR_Init();
	MS5837_init();
   

    while (1)
	{
		workstate=Get_work_state();
		
		if(workstate == 0)
		{
		    Start();
		}
		if(workstate==1)
		{
			/////////////////////////////////排水吸水运动
			// 排水
            if (Left_Move == water_push)//Left_2Hand_Right			
			{
                motor_control(1, 150, 8000);
            }

            // 吸水
            if (Right_Move == water_absorption) //Right_2Hand_Left
			{
                motor_control(0, 150, 8000);
			}
			
			/////////////////////////////////基础方向运动
		    // 前进(OK)
            if (key == Front) 
			{
                MoveForward();
            }

            // 后退(OK)
            if (key == Back)
			{
                MoveBackward();
            }

            // 向左平移
            if (key == Left)
			{
                MoveLeft();
            }

            // 向右平移
            if (key == Right) 
			{
                MoveRight();
            }

            // 向左转
            if (key == Left_Spin) //Left_2Hand_Left
			{
                TurnLeft();
            }

            // 向右转
            if (key == Right_Spin)//Right_2Hand_Right
			{
                TurnRight();
            }
            // 向上  （OK）
            if (key == Up)
			{    
                Ascend();
            }

            // 向下   （OK）
            if (key == Down)
			{
                Descend();
            }

			
			
		//主要任务处理
		   ////////////////////机械臂运动
			//左夹爪打开(按下为关，松开为放)  （OK）
			if (hand_OC == Left_Hand_Open) 
			{
                LeftHand_Open();
            }

		   //右夹爪打开(按下为关，松开为放)   （OK）
			if (hand_OC == Right_Hand_Open) 
			{
                RightHand_Open();
            }
			
			//左臂舒展
			if (Left_Move == Left_ARM_OPEN) //Left_1Hand_Left
			{
                LeftARM_Open();
            }
			//右臂舒展
			if (Right_Move == Right_ARM_OPEN) //Left_1Hand_Right
			{
                RightARM_Open();
            }

		}
		  
    }
}



