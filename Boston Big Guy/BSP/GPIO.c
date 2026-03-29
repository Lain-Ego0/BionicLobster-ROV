//此部分为GPIO初始化
#include "GPIO.h"

void MYGPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin  = 	GPIO_Pin_0|GPIO_Pin_1|							//TIM2CH 12
									GPIO_Pin_2|GPIO_Pin_3|							//usart TX RX		
									GPIO_Pin_6|GPIO_Pin_7|							//TIM3CH 12
									GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;	//TIM1CH 1234
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);//注意复用！！！！！
	GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_10|GPIO_Pin_11|						//TIM2CH 34
									GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|	//TIM4CH 1234
									GPIO_Pin_1;	//TIM3CH 4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	

	
}

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

//TIM3CH 12 4
/*PA6 PA7  PB1*/
//TIM4CH 2
/*PB7*/
/*
PA6		尾部上舵机
PA7		尾部下舵机
PB7		左上电机 
PB1		右上电机
*/

//TIM4CH 1 34
/*PB6   PB8 PB9*/
/*
PB6 	头部电机
		尾部电机1（无电机）
PB8 	尾部电机3
PB9		尾部电机2
*/
