#include "led.h"
#include "delay.h"

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}
//01控制LED灯亮暗
void LED_Control(unsigned char LED1,unsigned char LED2,unsigned char LED3,unsigned char LED4)
{
	if(LED1 == 0)GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	else GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
	if(LED2 == 0)GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	else GPIO_SetBits(GPIOB,GPIO_Pin_13);

	if(LED3 == 0)GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	else GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	if(LED4 == 0)GPIO_ResetBits(GPIOB,GPIO_Pin_15);
	else GPIO_SetBits(GPIOB,GPIO_Pin_15);
}

//特效流水灯
void running_water_lamp(unsigned char time_scope)
{
	LED_Control(1,0,0,0);
	delay_ms(time_scope);
	LED_Control(0,1,0,0);
	delay_ms(time_scope);
	LED_Control(0,0,1,0);
	delay_ms(time_scope);
	LED_Control(0,0,0,1);
	delay_ms(time_scope);
	LED_Control(1,1,1,1);
}
