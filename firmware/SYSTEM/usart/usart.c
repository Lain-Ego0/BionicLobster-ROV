#include "usart.h"
#include "main.h"

typedef struct
{
    uint8_t buffer[CONTROL_FRAME_SIZE];
    uint8_t index;
    uint8_t receiving;
} ControlFrameParser;

static ControlFrameParser g_remote_parser;

static void USART_SendByteBlocking(USART_TypeDef *usart, uint8_t byte)
{
    USART_SendData(usart, byte);
    while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET)
    {
    }
}

static void USART_SendArrayBlocking(USART_TypeDef *usart, const uint8_t *array, uint16_t length)
{
    uint16_t i;

    for (i = 0; i < length; i++)
    {
        USART_SendByteBlocking(usart, array[i]);
    }
}

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

static void ControlParser_Reset(ControlFrameParser *parser)
{
    parser->index = 0;
    parser->receiving = 0;
}

static void ApplyRemoteFrame(const uint8_t *frame)
{
    key = frame[1];
    Left_Move = frame[2];
    Right_Move = frame[3];
    hand_OC = frame[4];
    Water_Control = frame[5];
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
            ApplyRemoteFrame(parser->buffer);
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

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB, ENABLE);

    gpio_init_structure.GPIO_Pin = GPIO_Pin_6;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio_init_structure);

    gpio_init_structure.GPIO_Pin = GPIO_Pin_7;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpio_init_structure);

    USART_CommonInit(USART1, bound);
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
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 3;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);

    USART_CommonInit(USART2, bound);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

void Serial_SendByte(uint8_t byte)
{
    USART_SendByteBlocking(USART2, byte);
}

void Control_ResetState(void)
{
    key = 0x00;
    hand_OC = 0x00;
    Left_Move = 0x00;
    Right_Move = 0x00;
    Water_Control = 0x00;
}

void BoardLink_SendFrame(void)
{
    const uint8_t frame[CONTROL_FRAME_SIZE] =
    {
        Start_byte,
        key,
        Left_Move,
        Right_Move,
        hand_OC,
        Water_Control,
        End_byte
    };

    USART_SendArrayBlocking(USART1, frame, CONTROL_FRAME_SIZE);
}

void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        ControlParser_Push(&g_remote_parser, (uint8_t)USART_ReceiveData(USART2));
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

