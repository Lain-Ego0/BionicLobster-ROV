// 从板 GPIO 初始化
#include "GPIO.h"

void MYGPIO_Init(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);

    gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init_structure.GPIO_Pin =
        GPIO_Pin_0 | GPIO_Pin_1 |
        GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init_structure);

    gpio_init_structure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOB, &gpio_init_structure);
}
