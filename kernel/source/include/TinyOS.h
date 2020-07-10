#ifndef TINYOS_H
#define TINYOS_H

#include ".\tk_compiler.h"

#include ".\tConfig.h"
#include ".\tLib.h"
#include ".\tEvent.h"
#include ".\tTask.h"
#include ".\tSem.h"

#define TINYOS_TASK_STATE_RDY           0
#define TINYOS_TASK_STATE_DELAYED       (1u << 1)
#define TINYOS_TASK_STATE_DELETED       (1u << 2)
#define TINYOS_TASK_STATE_SUSPEND       (1u << 3)

#define TINYOS_WAIT_MASK                (0xFFFF0000)

#define TINYOS_SLICE_MAX                10

#define TASK_IDLE_ENV_SIZE  32

typedef enum {
    NO_ERROR = 0,
    TIMEOUT,
    RESOURCE_UNAVALIABLE
}tError;

extern tTask *currentTask;
extern tTask *nextTask;

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

extern void tTaskSystemTickHandler(void);

extern void tTimeTaskWait(tTask *task, uint32_t ticks);

extern void tTimeTaskWakeUp(tTask *task);

extern void tTaskSchedRemove(tTask* task);

extern void tTimeTaskRemove(tTask* task);

extern void tInitApp(void);

#endif
