#ifndef TTASK_H
#define TTASK_H

#include ".\tk_types.h"
#include ".\tLib.h"

typedef uint32_t tTaskStack;
typedef struct _tEvent tEvent;
typedef struct {
    tTaskStack* stack;
    uint32_t wDelayTicks;   // 任务延时计数器
    uint32_t prio;          // 优先级

    tNode delayNode;
    uint32_t state;

    tNode linkNode;
    uint32_t slice;         // 时间片

    uint32_t suspendCount;

    void (*clean)(void* param);
    void* cleanParam;
    uint8_t requestDeleteFlag;


    tEvent* waitEvent;
    void* eventMsg;
    uint32_t waitEventResult;

} tTask;

typedef struct {
    uint32_t delayTicks;
    uint32_t prio;
    uint32_t state;
    uint32_t slice;
    uint32_t suspendCount;
}tTaskInfo;

extern void tTaskInit(tTask* task, void(*entry)(void*), void* param, uint32_t prio, tTaskStack* stack);

extern void tTaskGetInfo(tTask* task, tTaskInfo* info);

extern void tTaskSuspend(tTask* task);

extern void tTaskWakeUp(tTask* task);

extern void tTaskSetCleanCallFunc(tTask* task, void(*clean)(void* param), void* param);

extern void tTaskForceDelete(tTask* task);

extern void tTaskRequestDelete(tTask* task);

extern uint8_t tTaskIsRequestDeleted(void);

extern void tTaskDeleteSelf(void);

#endif // !TTASK_H
