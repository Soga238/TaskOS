#ifndef TINYOS_H
#define TINYOS_H

#include ".\tk_compiler.h"
#include ".\tk_types.h"

#include ".\tLib.h"

typedef uint32_t tTaskStack;

typedef struct {
    tTaskStack* stack;
    uint32_t wDelayTicks;   // 任务延时计数器
} tTask;

extern tTask* currentTask;
extern tTask* nextTask;

extern void tTaskSwitch(void);
extern void tTaskRunFirst(void);

extern uint32_t tTaskEnterCritical(void);
extern void tTaskExitCritical(uint32_t status);

extern void tTaskSchedInit(void);
extern void tTaskSchedDisable(void);
extern void tTaskSchedEnable(void);

#endif
