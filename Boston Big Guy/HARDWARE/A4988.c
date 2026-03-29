#include "A4988.h"
/*
步进电机初始化，低电平使能
*/
#define Motor_STEP	  GPIO_Pin_5	//STEP - PA5
#define Motor_DIR	  GPIO_Pin_4	//DIR  - PA4
#define Motor_GPIO  GPIOA
void MOTOR_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开启GPIOA的时钟
	
	GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4  ;  //PA5->step;PA4->DIR;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_4);
	
//	GPIO_SetBits(Motor_GPIO,Motor_STEP);			//初始化GPIOB_6输出低电平
//	GPIO_SetBits(Motor_GPIO,Motor_DIR);
	GPIO_ResetBits(Motor_GPIO,Motor_DIR);			//初始化GPIOB_7输出低电平
	GPIO_ResetBits(Motor_GPIO,Motor_STEP);
}
/*
motor_dir:电机运动的方向 0/1
num:电机需要转动的圈数
speed：脉冲高低电平时间
周期=num*speed
*/
int test_look=0;
int test_look1=0;

void motor_control(unsigned int motor_dir,unsigned int num,unsigned int speed)
{
	test_look1=GPIO_ReadOutputDataBit(Motor_GPIO,Motor_DIR);
	test_look=GPIO_ReadOutputDataBit(Motor_GPIO,Motor_STEP);
		switch(motor_dir)
		{
			case 0 : GPIO_SetBits(Motor_GPIO,Motor_DIR); break; 
			case 1 : GPIO_ResetBits(Motor_GPIO,Motor_DIR); break; 
			default : break; 
		}
		
		for(uint32_t i=0;i<=(num*3200);i++)  //电平翻转2次为一个脉冲，所以旋转一周需要3200个脉冲(细分倍数是16，实际步距角为0.1125°,360/0.1125=3200）
		{		
			delay_us(speed);                   //高低电平持续时间=speed/num*3200
			GPIOA->ODR ^= GPIO_Pin_5;  				 //翻转PA5输出电平
//			GPIO_ToggleBits(Motor_GPIO,Motor_STEP);
			
		}	
		
}

