#pragma once

#include ".\tk_compiler.h"
#include ".\tk_types.h"

typedef uint32_t tTaskStack;

typedef struct {
    tTaskStack* stack;
    uint32_t delayTicks;
} tTask;

extern tTask* currentTask;
extern tTask* nextTask;

extern void tTaskSwitch(void);
extern void tTaskRunFirst(void);
