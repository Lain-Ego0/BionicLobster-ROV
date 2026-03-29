#include "usart.h"
#include "main.h"

typedef struct
{
    uint8_t buffer[CONTROL_FRAME_SIZE];
    uint8_t index;
    uint8_t receiving;
} ControlFrameParser;

static ControlFrameParser g_board_parser;
static volatile uint32_t g_board_link_age = BOARD_LINK_TIMEOUT_TICKS + 1U;

static void USART_CommonInit(USART_TypeDef *usart, uint32_t baud_rate)
{
    USART_InitTypeDef usart_init_structure;

    usart_init_structure.USART_BaudRate = baud_rate;
    usart_init_structure.USART_WordLength = USART_WordLength_8b;
    usart_init_structure.USART_StopBits = USART_StopBits_1;
    usart_init_structure.USART_Parity = USART_Parity_No;
    usart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init_structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(usart, &usart_init_structure);
}

static void USART_SendByteBlocking(USART_TypeDef *usart, uint8_t byte)
{
    USART_SendData(usart, byte);
    while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET)
    {
    }
}

static void ControlParser_Reset(ControlFrameParser *parser)
{
    parser->index = 0;
    parser->receiving = 0;
}

static void ApplyBoardFrame(const uint8_t *frame)
{
    key = frame[1];
    Left_Move = frame[2];
    Right_Move = frame[3];
    hand_OC = frame[4];
    Water_Control = frame[5];
    g_board_link_age = 0;
}

static void ControlParser_Push(ControlFrameParser *parser, uint8_t byte)
{
    if (parser->receiving == 0)
    {
        if (byte == Start_byte)
        {
            parser->receiving = 1;
            parser->index = 0;
            parser->buffer[parser->index++] = byte;
        }
        return;
    }

    if (parser->index >= CONTROL_FRAME_SIZE)
    {
        ControlParser_Reset(parser);
        return;
    }

    parser->buffer[parser->index++] = byte;

    if (parser->index == CONTROL_FRAME_SIZE)
    {
        if (parser->buffer[0] == Start_byte && parser->buffer[CONTROL_FRAME_SIZE - 1] == End_byte)
        {
            ApplyBoardFrame(parser->buffer);
        }
        else
        {
            Control_ResetState();
        }
        ControlParser_Reset(parser);
    }
}

void uart_init1(u32 bound)
{
    GPIO_InitTypeDef gpio_init_structure;
    NVIC_InitTypeDef nvic_init_structure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB, ENABLE);

    gpio_init_structure.GPIO_Pin = GPIO_Pin_6;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio_init_structure);

    gpio_init_structure.GPIO_Pin = GPIO_Pin_7;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpio_init_structure);

    nvic_init_structure.NVIC_IRQChannel = USART1_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 1;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);

    USART_CommonInit(USART1, bound);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

void uart_init2(u32 bound)
{
    GPIO_InitTypeDef gpio_init_structure;
    NVIC_InitTypeDef nvic_init_structure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio_init_structure.GPIO_Pin = GPIO_Pin_2;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio_init_structure);

    gpio_init_structure.GPIO_Pin = GPIO_Pin_3;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_init_structure);

    nvic_init_structure.NVIC_IRQChannel = USART2_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 2;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);

    USART_CommonInit(USART2, bound);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

void Serial_SendByte(uint8_t byte)
{
    USART_SendByteBlocking(USART1, byte);
}

void Control_ResetState(void)
{
    key = 0x00;
    hand_OC = 0x00;
    Left_Move = 0x00;
    Right_Move = 0x00;
    Water_Control = 0x00;
}

void BoardLink_Tick(void)
{
    if (g_board_link_age < 0xFFFFFFFFU)
    {
        g_board_link_age++;
    }
}

uint8_t BoardLink_IsOnline(void)
{
    return (g_board_link_age < BOARD_LINK_TIMEOUT_TICKS) ? 1U : 0U;
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        ControlParser_Push(&g_board_parser, (uint8_t)USART_ReceiveData(USART1));
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        CopeSerial2Data((uint8_t)USART_ReceiveData(USART2));
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}
