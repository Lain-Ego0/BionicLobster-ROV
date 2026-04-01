#include "Scheduler.h"
#include "stm32f10x.h"

#define SCHEDULER_TIM_PRESCALER          71u
#define SCHEDULER_TIM_PERIOD             999u
#define SCHEDULER_CONTROL_PERIOD_MS      10u
#define SCHEDULER_SUPERVISOR_PERIOD_MS   20u
#define SCHEDULER_STATUS_PERIOD_MS       100u

static volatile uint32_t g_scheduler_tick_ms;
static volatile uint8_t g_scheduler_pending_tasks;

void Scheduler_Init(void)
{
    TIM_TimeBaseInitTypeDef tim_time_base_structure;
    NVIC_InitTypeDef nvic_init_structure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    tim_time_base_structure.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_time_base_structure.TIM_CounterMode = TIM_CounterMode_Up;
    tim_time_base_structure.TIM_Period = SCHEDULER_TIM_PERIOD;
    tim_time_base_structure.TIM_Prescaler = SCHEDULER_TIM_PRESCALER;
    tim_time_base_structure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &tim_time_base_structure);

    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    nvic_init_structure.NVIC_IRQChannel = TIM4_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 0;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);

    TIM_Cmd(TIM4, ENABLE);
}

void Scheduler_HandleTickISR(void)
{
    static uint8_t control_divider;
    static uint8_t supervisor_divider;
    static uint8_t status_divider;

    g_scheduler_tick_ms++;

    control_divider++;
    supervisor_divider++;
    status_divider++;

    if (control_divider >= SCHEDULER_CONTROL_PERIOD_MS)
    {
        control_divider = 0u;
        g_scheduler_pending_tasks |= QUAD_SCHED_TASK_CONTROL;
    }

    if (supervisor_divider >= SCHEDULER_SUPERVISOR_PERIOD_MS)
    {
        supervisor_divider = 0u;
        g_scheduler_pending_tasks |= QUAD_SCHED_TASK_SUPERVISOR;
    }

    if (status_divider >= SCHEDULER_STATUS_PERIOD_MS)
    {
        status_divider = 0u;
        g_scheduler_pending_tasks |= QUAD_SCHED_TASK_STATUS;
    }
}

uint8_t Scheduler_TakePendingTasks(void)
{
    uint8_t pending_tasks;

    __disable_irq();
    pending_tasks = g_scheduler_pending_tasks;
    g_scheduler_pending_tasks = 0u;
    __enable_irq();

    return pending_tasks;
}

uint32_t Scheduler_GetTickMs(void)
{
    uint32_t tick_ms;

    __disable_irq();
    tick_ms = g_scheduler_tick_ms;
    __enable_irq();

    return tick_ms;
}
