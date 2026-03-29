#include "IIC.h"
#include "delay.h"

static const SoftI2CBus g_default_i2c_bus = {GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_3};

static void MyI2C_EnableClock(GPIO_TypeDef *port)
{
    if (port == GPIOA)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }
    else if (port == GPIOB)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    }
}

static void MyI2C_ConfigOutput(GPIO_TypeDef *port, uint16_t pin)
{
    GPIO_InitTypeDef gpio_init_structure;

    gpio_init_structure.GPIO_Pin = pin;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &gpio_init_structure);
}

static void MyI2C_ConfigInput(GPIO_TypeDef *port, uint16_t pin)
{
    GPIO_InitTypeDef gpio_init_structure;

    gpio_init_structure.GPIO_Pin = pin;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IPU;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &gpio_init_structure);
}

static void MyI2C_W_SCL_Bus(const SoftI2CBus *bus, uint8_t bit_value)
{
    GPIO_WriteBit(bus->scl_port, bus->scl_pin, (BitAction)bit_value);
    delay_us(5);
}

static void MyI2C_W_SDA_Bus(const SoftI2CBus *bus, uint8_t bit_value)
{
    MyI2C_ConfigOutput(bus->sda_port, bus->sda_pin);
    GPIO_WriteBit(bus->sda_port, bus->sda_pin, (BitAction)bit_value);
    delay_us(5);
}

static uint8_t MyI2C_R_SDA_Bus(const SoftI2CBus *bus)
{
    uint8_t bit_value;

    MyI2C_ConfigInput(bus->sda_port, bus->sda_pin);
    delay_us(2);
    bit_value = GPIO_ReadInputDataBit(bus->sda_port, bus->sda_pin);
    delay_us(5);
    return bit_value;
}

void MyI2C_InitBus(const SoftI2CBus *bus)
{
    MyI2C_EnableClock(bus->scl_port);
    MyI2C_EnableClock(bus->sda_port);

    MyI2C_ConfigOutput(bus->scl_port, bus->scl_pin);
    MyI2C_ConfigOutput(bus->sda_port, bus->sda_pin);

    GPIO_SetBits(bus->scl_port, bus->scl_pin);
    GPIO_SetBits(bus->sda_port, bus->sda_pin);
    delay_ms(1);
}

void MyI2C_StartBus(const SoftI2CBus *bus)
{
    MyI2C_W_SDA_Bus(bus, 1);
    MyI2C_W_SCL_Bus(bus, 1);
    MyI2C_W_SDA_Bus(bus, 0);
    MyI2C_W_SCL_Bus(bus, 0);
}

void MyI2C_StopBus(const SoftI2CBus *bus)
{
    MyI2C_W_SDA_Bus(bus, 0);
    MyI2C_W_SCL_Bus(bus, 1);
    MyI2C_W_SDA_Bus(bus, 1);
}

void MyI2C_SendByteBus(const SoftI2CBus *bus, uint8_t byte)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SDA_Bus(bus, byte & (0x80U >> i));
        MyI2C_W_SCL_Bus(bus, 1);
        MyI2C_W_SCL_Bus(bus, 0);
    }
}

uint8_t MyI2C_ReceiveByteBus(const SoftI2CBus *bus)
{
    uint8_t i;
    uint8_t byte = 0;

    MyI2C_W_SDA_Bus(bus, 1);
    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SCL_Bus(bus, 1);
        if (MyI2C_R_SDA_Bus(bus) == 1)
        {
            byte |= (0x80U >> i);
        }
        MyI2C_W_SCL_Bus(bus, 0);
    }

    return byte;
}

void MyI2C_SendAckBus(const SoftI2CBus *bus, uint8_t ack_bit)
{
    MyI2C_W_SDA_Bus(bus, ack_bit);
    MyI2C_W_SCL_Bus(bus, 1);
    MyI2C_W_SCL_Bus(bus, 0);
}

uint8_t MyI2C_ReceiveAckBus(const SoftI2CBus *bus)
{
    uint8_t ack_bit;

    MyI2C_W_SDA_Bus(bus, 1);
    MyI2C_W_SCL_Bus(bus, 1);
    ack_bit = MyI2C_R_SDA_Bus(bus);
    MyI2C_W_SCL_Bus(bus, 0);
    return ack_bit;
}

uint8_t MYIIC_Wait_AckBus(const SoftI2CBus *bus)
{
    uint8_t timeout = 0;

    MyI2C_W_SDA_Bus(bus, 1);
    delay_us(5);
    MyI2C_W_SCL_Bus(bus, 1);
    delay_us(5);

    while (MyI2C_R_SDA_Bus(bus))
    {
        timeout++;
        if (timeout > 250)
        {
            MyI2C_StopBus(bus);
            return 1;
        }
    }

    MyI2C_W_SCL_Bus(bus, 0);
    return 0;
}

uint8_t IIC_Read_ByteBus(const SoftI2CBus *bus, unsigned char ack)
{
    unsigned char i;
    uint8_t receive = 0;

    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SCL_Bus(bus, 0);
        delay_us(5);
        MyI2C_W_SCL_Bus(bus, 1);
        receive <<= 1;
        if (MyI2C_R_SDA_Bus(bus))
        {
            receive++;
        }
        delay_us(5);
    }

    if (ack == 0)
    {
        MYIIC_NAckBus(bus);
    }
    else
    {
        MYIIC_AckBus(bus);
    }

    return receive;
}

void MYIIC_AckBus(const SoftI2CBus *bus)
{
    MyI2C_W_SCL_Bus(bus, 0);
    MyI2C_W_SDA_Bus(bus, 0);
    delay_us(5);
    MyI2C_W_SCL_Bus(bus, 1);
    delay_us(5);
    MyI2C_W_SCL_Bus(bus, 0);
}

void MYIIC_NAckBus(const SoftI2CBus *bus)
{
    MyI2C_W_SCL_Bus(bus, 0);
    MyI2C_W_SDA_Bus(bus, 1);
    delay_us(5);
    MyI2C_W_SCL_Bus(bus, 1);
    delay_us(5);
    MyI2C_W_SCL_Bus(bus, 0);
}

void MyI2C_Init(void)
{
    MyI2C_InitBus(&g_default_i2c_bus);
}

void MyI2C_Start(void)
{
    MyI2C_StartBus(&g_default_i2c_bus);
}

void MyI2C_Stop(void)
{
    MyI2C_StopBus(&g_default_i2c_bus);
}

void MyI2C_SendByte(uint8_t byte)
{
    MyI2C_SendByteBus(&g_default_i2c_bus, byte);
}

uint8_t MyI2C_ReceiveByte(void)
{
    return MyI2C_ReceiveByteBus(&g_default_i2c_bus);
}

void MyI2C_SendAck(uint8_t ack_bit)
{
    MyI2C_SendAckBus(&g_default_i2c_bus, ack_bit);
}

uint8_t MyI2C_ReceiveAck(void)
{
    return MyI2C_ReceiveAckBus(&g_default_i2c_bus);
}

uint8_t MYIIC_Wait_Ack(void)
{
    return MYIIC_Wait_AckBus(&g_default_i2c_bus);
}

uint8_t IIC_Read_Byte(unsigned char ack)
{
    return IIC_Read_ByteBus(&g_default_i2c_bus, ack);
}

void MYIIC_Ack(void)
{
    MYIIC_AckBus(&g_default_i2c_bus);
}

void MYIIC_NAck(void)
{
    MYIIC_NAckBus(&g_default_i2c_bus);
}
