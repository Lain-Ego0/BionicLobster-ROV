#include "usart.h"
#include "main.h"

typedef struct
{
    uint8_t buffer[REMOTE_CONTROL_FRAME_SIZE];
    uint8_t index;
    uint8_t receiving;
} ControlFrameParser;

typedef struct
{
    uint8_t buffer[TELEMETRY_LINK_FRAME_SIZE];
    uint8_t index;
    uint8_t receiving;
} TelemetryFrameParser;

static ControlFrameParser g_remote_parser;
static TelemetryFrameParser g_telemetry_parser;
static uint8_t g_board_link_sequence = 0u;
static volatile QuadTelemetryStatus g_quad_telemetry;
static volatile uint8_t g_quad_telemetry_online;
static volatile uint8_t g_quad_telemetry_pending_forward;
static uint8_t g_quad_telemetry_forward_frame[TELEMETRY_LINK_FRAME_SIZE];

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

static void TelemetryParser_Reset(TelemetryFrameParser *parser)
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

static void ApplyTelemetryFrame(const uint8_t *frame)
{
    uint8_t i;

    g_quad_telemetry.run_state = frame[TELEMETRY_LINK_RUN_STATE_INDEX];
    g_quad_telemetry.fault_flags = frame[TELEMETRY_LINK_FAULT_FLAGS_INDEX];
    g_quad_telemetry.board_link_online = frame[TELEMETRY_LINK_BOARD_LINK_INDEX];
    g_quad_telemetry.imu_online = frame[TELEMETRY_LINK_IMU_INDEX];
    g_quad_telemetry.control_mode = frame[TELEMETRY_LINK_MODE_INDEX];
    g_quad_telemetry.surge = (int8_t)frame[TELEMETRY_LINK_SURGE_INDEX];
    g_quad_telemetry.yaw = (int8_t)frame[TELEMETRY_LINK_YAW_INDEX];
    g_quad_telemetry.heave = (int8_t)frame[TELEMETRY_LINK_HEAVE_INDEX];
    g_quad_telemetry_online = 1u;

    for (i = 0; i < TELEMETRY_LINK_FRAME_SIZE; i++)
    {
        g_quad_telemetry_forward_frame[i] = frame[i];
    }
    g_quad_telemetry_pending_forward = 1u;
}

static void ControlParser_Push(ControlFrameParser *parser, uint8_t byte)
{
    if (parser->receiving == 0)
    {
        if (byte == REMOTE_CONTROL_START_BYTE)
        {
            parser->receiving = 1;
            parser->index = 0;
            parser->buffer[parser->index++] = byte;
        }
        return;
    }

    if (parser->index >= REMOTE_CONTROL_FRAME_SIZE)
    {
        ControlParser_Reset(parser);
        return;
    }

    parser->buffer[parser->index++] = byte;

    if (parser->index == REMOTE_CONTROL_FRAME_SIZE)
    {
        if (parser->buffer[0] == REMOTE_CONTROL_START_BYTE &&
            parser->buffer[REMOTE_CONTROL_FRAME_SIZE - 1] == REMOTE_CONTROL_END_BYTE)
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

static void TelemetryParser_Push(TelemetryFrameParser *parser, uint8_t byte)
{
    if (parser->receiving == 0)
    {
        if (byte == TELEMETRY_LINK_START_BYTE)
        {
            parser->receiving = 1;
            parser->index = 0;
            parser->buffer[parser->index++] = byte;
        }
        return;
    }

    if (parser->index >= TELEMETRY_LINK_FRAME_SIZE)
    {
        TelemetryParser_Reset(parser);
        return;
    }

    parser->buffer[parser->index++] = byte;

    if (parser->index == TELEMETRY_LINK_FRAME_SIZE)
    {
        if (parser->buffer[TELEMETRY_LINK_START_INDEX] == TELEMETRY_LINK_START_BYTE &&
            parser->buffer[TELEMETRY_LINK_END_INDEX] == TELEMETRY_LINK_END_BYTE &&
            parser->buffer[TELEMETRY_LINK_VERSION_INDEX] == TELEMETRY_LINK_VERSION &&
            parser->buffer[TELEMETRY_LINK_CHECKSUM_INDEX] == TelemetryLink_CalculateChecksum(parser->buffer))
        {
            ApplyTelemetryFrame(parser->buffer);
        }
        TelemetryParser_Reset(parser);
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
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 2;
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

uint8_t QuadTelemetry_IsOnline(void)
{
    return g_quad_telemetry_online;
}

void QuadTelemetry_GetStatus(QuadTelemetryStatus *status)
{
    uint8_t run_state;
    uint8_t fault_flags;
    uint8_t board_link_online;
    uint8_t imu_online;
    uint8_t control_mode;
    int8_t surge;
    int8_t yaw;
    int8_t heave;

    if (status == NULL)
    {
        return;
    }

    __disable_irq();
    run_state = g_quad_telemetry.run_state;
    fault_flags = g_quad_telemetry.fault_flags;
    board_link_online = g_quad_telemetry.board_link_online;
    imu_online = g_quad_telemetry.imu_online;
    control_mode = g_quad_telemetry.control_mode;
    surge = g_quad_telemetry.surge;
    yaw = g_quad_telemetry.yaw;
    heave = g_quad_telemetry.heave;
    __enable_irq();

    status->run_state = run_state;
    status->fault_flags = fault_flags;
    status->board_link_online = board_link_online;
    status->imu_online = imu_online;
    status->control_mode = control_mode;
    status->surge = surge;
    status->yaw = yaw;
    status->heave = heave;
}

void QuadTelemetry_FlushToHost(void)
{
    uint8_t frame_copy[TELEMETRY_LINK_FRAME_SIZE];
    uint8_t i;

    __disable_irq();
    if (g_quad_telemetry_pending_forward == 0u)
    {
        __enable_irq();
        return;
    }

    g_quad_telemetry_pending_forward = 0u;
    for (i = 0; i < TELEMETRY_LINK_FRAME_SIZE; i++)
    {
        frame_copy[i] = g_quad_telemetry_forward_frame[i];
    }
    __enable_irq();

    USART_SendArrayBlocking(USART2, frame_copy, TELEMETRY_LINK_FRAME_SIZE);
}

void BoardLink_SendCommand(const BoardControlCommand *command)
{
    BoardControlCommand safe_command;
    uint8_t frame[BOARD_LINK_FRAME_SIZE];

    if (command == NULL)
    {
        safe_command.mode = BOARD_CONTROL_MODE_SAFE;
        safe_command.surge = 0;
        safe_command.yaw = 0;
        safe_command.heave = 0;
        safe_command.flags = BOARD_CONTROL_FLAG_NONE;
        command = &safe_command;
    }

    frame[BOARD_LINK_START_INDEX] = BOARD_LINK_START_BYTE;
    frame[BOARD_LINK_VERSION_INDEX] = BOARD_LINK_VERSION;
    frame[BOARD_LINK_SEQUENCE_INDEX] = g_board_link_sequence++;
    frame[BOARD_LINK_MODE_INDEX] = command->mode;
    frame[BOARD_LINK_SURGE_INDEX] = (uint8_t)command->surge;
    frame[BOARD_LINK_YAW_INDEX] = (uint8_t)command->yaw;
    frame[BOARD_LINK_HEAVE_INDEX] = (uint8_t)command->heave;
    frame[BOARD_LINK_FLAGS_INDEX] = command->flags;
    frame[BOARD_LINK_CHECKSUM_INDEX] = BoardLink_CalculateChecksum(frame);
    frame[BOARD_LINK_END_INDEX] = BOARD_LINK_END_BYTE;

    USART_SendArrayBlocking(USART1, frame, BOARD_LINK_FRAME_SIZE);
}

void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        ControlParser_Push(&g_remote_parser, (uint8_t)USART_ReceiveData(USART2));
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        TelemetryParser_Push(&g_telemetry_parser, (uint8_t)USART_ReceiveData(USART1));
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

