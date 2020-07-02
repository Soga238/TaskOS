#ifndef TINYOS_H
#define TINYOS_H

#include ".\tk_compiler.h"
#include ".\tk_types.h"

#include ".\tConfig.h"
#include ".\tLib.h"

typedef uint32_t tTaskStack;

#define TINYOS_TASK_STATE_RDY           0
#define TINYOS_TASK_STATE_DELAYED       (1u << 1)
#define TINYOS_TASK_STATE_SUSPEND       (1u << 2)

#define TINYOS_SLICE_MAX                10

#define TASK_IDLE_ENV_SIZE  32

typedef struct {
    tTaskStack *stack;
    uint32_t wDelayTicks;   // 任务延时计数器
    uint32_t prio;          // 优先级

    tNode delayNode;
    uint32_t state;

    tNode linkNode;
    uint32_t slice;         // 时间片

    uint32_t suspendCount;

} tTask;

extern tTask *currentTask;
extern tTask *nextTask;

extern void tTaskInit(tTask *task, void(*entry)(void *), void *param, uint32_t prio, tTaskStack *stack);

extern void tTaskSwitch(void);

extern void tTaskRunFirst(void);

extern uint32_t tTaskEnterCritical(void);

extern void tTaskExitCritical(uint32_t status);

extern void tTaskSchedInit(void);

extern void tTaskSched(void);

extern void tTaskSchedDisable(void);

extern void tTaskSchedEnable(void);

extern void tTaskScedRdy(tTask *task);

extern void tTaskScedUnRdy(tTask *task);

extern void tTaskDelay(uint32_t wTicks);

extern void tTaskSuspend(tTask* task);

extern void tTaskWakeUp(tTask* task);

extern void tTaskSystemTickHandler(void);

extern void tTimeTaskWait(tTask *task, uint32_t ticks);

extern void tTimeTaskWakeUp(tTask *task);

extern void tInitApp(void);

#endif
