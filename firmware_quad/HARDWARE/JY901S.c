#include "JY901S.h"
#include "string.h"

#define JY901S_TIMEOUT_TICKS 20U

struct STime stcTime;
struct SAcc stcAcc;
struct SGyro stcGyro;
struct SAngle stcAngle;
struct SMag stcMag;
struct SDStatus stcDStatus;
struct SPress stcPress;
struct SLonLat stcLonLat;
struct SGPSV stcGPSV;
struct SQ stcQ;

static uint8_t g_jy901s_angle_ready;
static uint8_t g_jy901s_gyro_ready;
static uint8_t g_jy901s_age_ticks;
static unsigned char g_rx_buffer[11];
static unsigned char g_rx_count;

void sendcmd(char *cmd)
{
    int i;

    for (i = 0; i < 5; i++)
    {
        UART2_Put_Char((unsigned char)cmd[i]);
    }
}

void UART2_Put_Char(unsigned char data_to_send)
{
    USART_SendData(USART2, data_to_send);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
    {
    }
}

void CopeSerial2Data(unsigned char ucData)
{
    uint8_t checksum = 0;
    uint8_t i;

    if (g_rx_count == 0 && ucData != 0x55)
    {
        return;
    }

    g_rx_buffer[g_rx_count++] = ucData;
    if (g_rx_count < 11)
    {
        return;
    }

    for (i = 0; i < 10; i++)
    {
        checksum += g_rx_buffer[i];
    }

    if (checksum == g_rx_buffer[10])
    {
        g_jy901s_age_ticks = 0;

        switch (g_rx_buffer[1])
        {
            case 0x50: memcpy(&stcTime, &g_rx_buffer[2], 8); break;
            case 0x51: memcpy(&stcAcc, &g_rx_buffer[2], 8); break;
            case 0x52:
                memcpy(&stcGyro, &g_rx_buffer[2], 8);
                g_jy901s_gyro_ready = 1;
                break;
            case 0x53:
                memcpy(&stcAngle, &g_rx_buffer[2], 8);
                g_jy901s_angle_ready = 1;
                break;
            case 0x54: memcpy(&stcMag, &g_rx_buffer[2], 8); break;
            case 0x55: memcpy(&stcDStatus, &g_rx_buffer[2], 8); break;
            case 0x56: memcpy(&stcPress, &g_rx_buffer[2], 8); break;
            case 0x57: memcpy(&stcLonLat, &g_rx_buffer[2], 8); break;
            case 0x58: memcpy(&stcGPSV, &g_rx_buffer[2], 8); break;
            case 0x59: memcpy(&stcQ, &g_rx_buffer[2], 8); break;
            default: break;
        }
    }

    g_rx_count = 0;
}

void JY901S_Tick(void)
{
    if (g_jy901s_age_ticks < 0xFFU)
    {
        g_jy901s_age_ticks++;
    }
}

uint8_t JY901S_AngleReady(void)
{
    return g_jy901s_angle_ready;
}

uint8_t JY901S_IsOnline(void)
{
    if (g_jy901s_angle_ready == 0 || g_jy901s_gyro_ready == 0)
    {
        return 0;
    }

    return (g_jy901s_age_ticks < JY901S_TIMEOUT_TICKS) ? 1U : 0U;
}

float JY901S_GetRollDeg(void)
{
    return stcAngle.Angle[0] / 32768.0f * 180.0f;
}

float JY901S_GetPitchDeg(void)
{
    return stcAngle.Angle[1] / 32768.0f * 180.0f;
}

float JY901S_GetYawDeg(void)
{
    return stcAngle.Angle[2] / 32768.0f * 180.0f;
}

float JY901S_GetRollRateDegPerSec(void)
{
    return stcGyro.w[0] / 32768.0f * 2000.0f;
}

float JY901S_GetPitchRateDegPerSec(void)
{
    return stcGyro.w[1] / 32768.0f * 2000.0f;
}

float JY901S_GetYawRateDegPerSec(void)
{
    return stcGyro.w[2] / 32768.0f * 2000.0f;
}
