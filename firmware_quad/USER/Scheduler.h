#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <stdint.h>

#define QUAD_SCHED_TASK_CONTROL      0x01u
#define QUAD_SCHED_TASK_SUPERVISOR   0x02u
#define QUAD_SCHED_TASK_STATUS       0x04u

void Scheduler_Init(void);
void Scheduler_HandleTickISR(void);
uint8_t Scheduler_TakePendingTasks(void);
uint32_t Scheduler_GetTickMs(void);

#endif
