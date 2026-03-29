//江科大蓝牙模块
#include "sys.h"
#include "usart.h"	  
#include "stm32f10x.h"                  
#include "stdarg.h"
#include "main.h"

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

void uart_init2(u32 bound){
    // GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);   // 使能USART2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    // 使能GPIOA时钟
    
    // USART2_TX   GPIOA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIOA.2
    
    // USART2_RX   GPIOA3初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIOA.3  
    
    // Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); // 根据指定的参数初始化VIC寄存器
    
    // USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound; // 串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USART2, &USART_InitStructure); // 初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启串口接受中断
    USART_Cmd(USART2, ENABLE); // 使能串口2
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART2, Byte);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}


uint32_t data_index = 0;								//用于跟踪当前数据接收到的位置索引
_Bool is_receiving = 0;									//标志位，用于表示是否处于数据接收状态
_Bool over_receiving = 0;								//标志位，用于表示是否接收到了数据帧的结束字节
uint8_t uart_rx_buffer[UART_BUFFER_SIZE]; 				// 接收缓冲区
uint8_t temp_data[7];									//用来存放4组8位数字（每组两位）
uint8_t Remote_control_data[7]; 						// 接收7个字节数据
uint8_t last_received_data[7]; 							// 接收7个字节数据


/*USART2中断处理程序*/
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t temp_data = USART_ReceiveData(USART2);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);

        if (temp_data == Start_byte) // 如果接收到起始字节
        {
            is_receiving = 1; // 标记开始接收
            data_index = 0;   // 重置数据索引
        }
        else if (temp_data == End_byte) // 如果接收到结束字节
        {
            over_receiving = 1; // 标记接收结束
        }

        if (is_receiving) // 正在接收数据
        {
            if (data_index < UART_BUFFER_SIZE)
            {
                uart_rx_buffer[data_index++] = temp_data; // 存储数据
            }
            else // 缓冲区溢出，重置状态
            {
                reset_receive_state();
            }
        }

        if (!is_receiving) // 如果未接收到新数据
        {
            copy_last_data(); // 复制上次接收到的数据
        }

        if (over_receiving == 1 && is_receiving == 1) // 接收到完整数据帧
        {
            process_data(); // 处理数据
            is_receiving = 0; // 重置接收状态
            over_receiving = 0; // 重置结束状态
        }

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}


void reset_receive_state(void)
{
    data_index = 0;
    is_receiving = 0;
}
			
void copy_last_data(void)
{
    // 复制上一次接收到的last_received_data数据数组
    for (int i = 0; i < 4; i++) 
	  {
        Remote_control_data[i] = last_received_data[i];
    }
}
void process_data(void)
{
	
    for (int j = 0; j < 7; j++)
    {
        temp_data[j] = uart_rx_buffer[j];
    }

    if (temp_data[1]!= 0x00||temp_data[2]!= 0x00||temp_data[3]!= 0x00||temp_data[4]!= 0x00)
    {
		key = temp_data[1];
		hand_OC=temp_data[4];
		Left_Move=temp_data[2];
		Right_Move=temp_data[3];
			
//				data_keep[0]=key;
//				data_keep[1]=hand_OC;
//				data_keep[2]=Left_Move;
//				data_keep[3]=Right_Move;
			
//        key_number[0] = (key == 0x01)? 1 : 0;//前
//        key_number[1] = (key == 0x02)? 1 : 0;//后
//        key_number[2] = (key == 0x04)? 1 : 0;//左
//        key_number[3] = (key == 0x08)? 1 : 0;//右
//        key_number[4] = (key == 0x10)? 1 : 0;//上
//        key_number[5] = (key == 0x20)? 1 : 0;//下
//				key_number[5] = (key == 0x20)? 1 : 0;//左开
    }
		//OPEN
    else
    {
				key = 0x00;
				hand_OC=0x00;
				Left_Move=0x00;
				Right_Move=0x00;
    }

	}

