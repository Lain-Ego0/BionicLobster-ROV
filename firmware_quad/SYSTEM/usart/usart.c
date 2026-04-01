#include "usart.h"
#include "main.h"

typedef struct
{
    uint8_t buffer[BOARD_LINK_FRAME_SIZE];
    uint8_t index;
    uint8_t receiving;
} ControlFrameParser;

static ControlFrameParser g_board_parser;
static volatile uint32_t g_board_link_age = BOARD_LINK_TIMEOUT_TICKS + 1U;
static volatile BoardControlCommand g_board_command;
static uint8_t g_telemetry_sequence = 0u;

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

static void USART_SendArrayBlocking(USART_TypeDef *usart, const uint8_t *array, uint16_t length)
{
    uint16_t i;

    for (i = 0; i < length; i++)
    {
        USART_SendByteBlocking(usart, array[i]);
    }
}

static void ApplyBoardFrame(const uint8_t *frame)
{
    g_board_command.mode = frame[BOARD_LINK_MODE_INDEX];
    g_board_command.surge = (int8_t)frame[BOARD_LINK_SURGE_INDEX];
    g_board_command.yaw = (int8_t)frame[BOARD_LINK_YAW_INDEX];
    g_board_command.heave = (int8_t)frame[BOARD_LINK_HEAVE_INDEX];
    g_board_command.flags = frame[BOARD_LINK_FLAGS_INDEX];
    g_board_link_age = 0;
}

static void ControlParser_Push(ControlFrameParser *parser, uint8_t byte)
{
    if (parser->receiving == 0)
    {
        if (byte == BOARD_LINK_START_BYTE)
        {
            parser->receiving = 1;
            parser->index = 0;
            parser->buffer[parser->index++] = byte;
        }
        return;
    }

    if (parser->index >= BOARD_LINK_FRAME_SIZE)
    {
        ControlParser_Reset(parser);
        return;
    }

    parser->buffer[parser->index++] = byte;

    if (parser->index == BOARD_LINK_FRAME_SIZE)
    {
        if (parser->buffer[BOARD_LINK_START_INDEX] == BOARD_LINK_START_BYTE &&
            parser->buffer[BOARD_LINK_END_INDEX] == BOARD_LINK_END_BYTE &&
            parser->buffer[BOARD_LINK_VERSION_INDEX] == BOARD_LINK_VERSION &&
            parser->buffer[BOARD_LINK_CHECKSUM_INDEX] == BoardLink_CalculateChecksum(parser->buffer))
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
    g_board_command.mode = BOARD_CONTROL_MODE_SAFE;
    g_board_command.surge = 0;
    g_board_command.yaw = 0;
    g_board_command.heave = 0;
    g_board_command.flags = BOARD_CONTROL_FLAG_NONE;
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

void BoardLink_GetControlCommand(BoardControlCommand *command)
{
    if (command == NULL)
    {
        return;
    }

    command->mode = g_board_command.mode;
    command->surge = g_board_command.surge;
    command->yaw = g_board_command.yaw;
    command->heave = g_board_command.heave;
    command->flags = g_board_command.flags;
}

void TelemetryLink_SendStatus(const QuadTelemetryStatus *status)
{
    QuadTelemetryStatus safe_status;
    uint8_t frame[TELEMETRY_LINK_FRAME_SIZE];

    if (status == NULL)
    {
        safe_status.run_state = 0u;
        safe_status.fault_flags = 0u;
        safe_status.board_link_online = 0u;
        safe_status.imu_online = 0u;
        safe_status.control_mode = BOARD_CONTROL_MODE_SAFE;
        safe_status.surge = 0;
        safe_status.yaw = 0;
        safe_status.heave = 0;
        status = &safe_status;
    }

    frame[TELEMETRY_LINK_START_INDEX] = TELEMETRY_LINK_START_BYTE;
    frame[TELEMETRY_LINK_VERSION_INDEX] = TELEMETRY_LINK_VERSION;
    frame[TELEMETRY_LINK_SEQUENCE_INDEX] = g_telemetry_sequence++;
    frame[TELEMETRY_LINK_RUN_STATE_INDEX] = status->run_state;
    frame[TELEMETRY_LINK_FAULT_FLAGS_INDEX] = status->fault_flags;
    frame[TELEMETRY_LINK_BOARD_LINK_INDEX] = status->board_link_online;
    frame[TELEMETRY_LINK_IMU_INDEX] = status->imu_online;
    frame[TELEMETRY_LINK_MODE_INDEX] = status->control_mode;
    frame[TELEMETRY_LINK_SURGE_INDEX] = (uint8_t)status->surge;
    frame[TELEMETRY_LINK_YAW_INDEX] = (uint8_t)status->yaw;
    frame[TELEMETRY_LINK_HEAVE_INDEX] = (uint8_t)status->heave;
    frame[TELEMETRY_LINK_CHECKSUM_INDEX] = TelemetryLink_CalculateChecksum(frame);
    frame[TELEMETRY_LINK_END_INDEX] = TELEMETRY_LINK_END_BYTE;

    USART_SendArrayBlocking(USART1, frame, TELEMETRY_LINK_FRAME_SIZE);
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
