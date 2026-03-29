#include "timer.h"

void PWM_ParameterConfiguration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    // 使能定时器和AFIO复用功能时钟    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  
    // TIM2部分重映射
    GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);

    // 配置定时器基础参数
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 19999; // 自动重装载值，配合预分频实现PWM频率
    TIM_TimeBaseStructure.TIM_Prescaler = 71;  // 预分频，计算得到PWM频率
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // 不重复计数
    // 初始化TIM2, TIM3, TIM4的时间基数配置
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		
		 TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 1999; // 自动重装载值，配合预分频实现PWM频率
    TIM_TimeBaseStructure.TIM_Prescaler = 71;  // 预分频，计算得到PWM频率
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // 不重复计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    // 配置输出比较（OC）模式，用于PWM输出
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // PWM模式2
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset; // 输出比较空闲状态为低电平
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; // 输出极性为低
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出比较状态	
    TIM_OCInitStructure.TIM_Pulse = 0; // 设置占空比初始值为0
	// 初始化TIM1所有通道的OC模式
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    // 初始化TIM2所有通道的OC模式
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    // 初始化TIM3所有通道的OC模式
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    // 初始化TIM4所有通道的OC模式
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    // 使能预装载寄存器，允许更改占空比而不产生延迟
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    // 使能自动重装载预装载
	TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//注意TIM1主输出模式
	
    // 启动定时器
	TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}




