#include "timer.h"

void PWM_ParameterConfiguration(void)
{
    TIM_TimeBaseInitTypeDef tim_time_base_structure;
    TIM_OCInitTypeDef tim_oc_init_structure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);

    tim_time_base_structure.TIM_ClockDivision = 0;
    tim_time_base_structure.TIM_CounterMode = TIM_CounterMode_Up;
    tim_time_base_structure.TIM_Period = 19999;
    tim_time_base_structure.TIM_Prescaler = 71;
    tim_time_base_structure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM1, &tim_time_base_structure);
    TIM_TimeBaseInit(TIM2, &tim_time_base_structure);
    TIM_TimeBaseInit(TIM3, &tim_time_base_structure);

    tim_oc_init_structure.TIM_OCMode = TIM_OCMode_PWM2;
    tim_oc_init_structure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    tim_oc_init_structure.TIM_OCPolarity = TIM_OCPolarity_Low;
    tim_oc_init_structure.TIM_OutputState = TIM_OutputState_Enable;
    tim_oc_init_structure.TIM_Pulse = 1500;

    TIM_OC1Init(TIM1, &tim_oc_init_structure);
    TIM_OC2Init(TIM1, &tim_oc_init_structure);
    TIM_OC3Init(TIM1, &tim_oc_init_structure);
    TIM_OC4Init(TIM1, &tim_oc_init_structure);

    TIM_OC1Init(TIM2, &tim_oc_init_structure);
    TIM_OC2Init(TIM2, &tim_oc_init_structure);
    TIM_OC3Init(TIM2, &tim_oc_init_structure);
    TIM_OC4Init(TIM2, &tim_oc_init_structure);

    TIM_OC1Init(TIM3, &tim_oc_init_structure);
    TIM_OC2Init(TIM3, &tim_oc_init_structure);
    TIM_OC3Init(TIM3, &tim_oc_init_structure);
    TIM_OC4Init(TIM3, &tim_oc_init_structure);

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

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}
