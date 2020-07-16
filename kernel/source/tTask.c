#include ".\include\TinyOS.h"
#include <string.h>

void tTaskInit(tTask *task, void(*entry)(void *), void *param, uint32_t prio, tTaskStack *stack, uint32_t size)
{
    uint32_t* stackTop;

    task->stackBase = stack;
    task->stackSize = size;
    memset(stack, 0, size);

    stackTop = stack + size / sizeof(tTaskStack);

    *(--stackTop) = (unsigned long)(1 << 24);              // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
    *(--stackTop) = (unsigned long)entry;                  // 程序的入口地址
    *(--stackTop) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--stackTop) = (unsigned long)0x12;                   // R12, 未用
    *(--stackTop) = (unsigned long)0x3;                    // R3, 未用
    *(--stackTop) = (unsigned long)0x2;                    // R2, 未用
    *(--stackTop) = (unsigned long)0x1;                    // R1, 未用
    *(--stackTop) = (unsigned long)param;                  // R0 = param, 传给任务的入口函数
    *(--stackTop) = (unsigned long)0x11;                   // R11, 未用
    *(--stackTop) = (unsigned long)0x10;                   // R10, 未用
    *(--stackTop) = (unsigned long)0x9;                    // R9, 未用
    *(--stackTop) = (unsigned long)0x8;                    // R8, 未用
    *(--stackTop) = (unsigned long)0x7;                    // R7, 未用
    *(--stackTop) = (unsigned long)0x6;                    // R6, 未用
    *(--stackTop) = (unsigned long)0x5;                    // R5, 未用
    *(--stackTop) = (unsigned long)0x4;                    // R4, 未用

    task->stack = stackTop;                                // 保存最终的值
    task->wDelayTicks = 0;
    task->prio = prio;

    task->state = TINYOS_TASK_STATE_RDY;
    tNodeInit(&task->delayNode);                        // 时间延时链表中

    task->slice = TINYOS_SLICE_MAX;
    tNodeInit(&task->linkNode);

    task->suspendCount = 0;

    task->clean = NULL;
    task->cleanParam = NULL;
    task->requestDeleteFlag = 0;

    tTaskScedRdy(task);
}

// 挂起任务，将优先级任务链表中删除
void tTaskSuspend(tTask *task)
{
    uint32_t status = tTaskEnterCritical();

    // 延时状态的任务不能被挂起
    if (!(task->state & TINYOS_TASK_STATE_SUSPEND)) {

        // 可能已经处于挂起状态了
        if (++task->suspendCount <= 1) {
            tTaskScedUnRdy(task);
            task->state |= TINYOS_TASK_STATE_SUSPEND;

            if (task == currentTask) {
                tTaskSched();
            }
        }
    }

    tTaskExitCritical(status);
}

// 恢复被挂起的任务
void tTaskWakeUp(tTask *task)
{
    uint32_t status = tTaskEnterCritical();

    // 延时状态的任务不能被挂起
    if (task->state & TINYOS_TASK_STATE_SUSPEND) {

        // 可能已经处于挂起状态了
        if (--task->suspendCount == 0) {
            tTaskScedRdy(task);
            task->state &= ~TINYOS_TASK_STATE_SUSPEND;

            // 触发一次任务调度，当前任务可能是最高优先级的任务
            tTaskSched();
        }
    }

    tTaskExitCritical(status);
}

void tTaskSetCleanCallFunc(tTask *task, void(*clean)(void *param), void *param)
{
    task->clean = clean;
    task->cleanParam = param;
}

void tTaskForceDelete(tTask *task)
{
    uint32_t status = tTaskEnterCritical();

    // 被删除的任务处于延时状态
    if (task->state & TINYOS_TASK_STATE_DELAYED) {
        tTimeTaskRemove(task);
    } else if (!(task->state & TINYOS_TASK_STATE_SUSPEND)) {
        tTaskSchedRemove(task);
    }

    if (task->clean) {
        task->clean(task->cleanParam);
    }

    if (currentTask == task) {
        tTaskSched();   // 删除自己后，立即调度任务
    }

    tTaskExitCritical(status);
}

void tTaskRequestDelete(tTask *task)
{
    uint32_t status = tTaskEnterCritical();

    task->requestDeleteFlag = 1;

    tTaskExitCritical(status);
}

uint8_t tTaskIsRequestDeleted(void)
{
    uint32_t status = tTaskEnterCritical();
    
    uint8_t flag = currentTask->requestDeleteFlag;

    tTaskExitCritical(status);

    return flag;
}

void tTaskDeleteSelf(void)
{
    uint32_t status = tTaskEnterCritical();

    tTaskSchedRemove(currentTask);

    if (currentTask->clean) {
        currentTask->clean(currentTask->cleanParam);
    }

    tTaskSched();   // 删除自己后，立即调度任务

    tTaskExitCritical(status);
}

void tTaskGetInfo(tTask *task, tTaskInfo *info)
{
    uint32_t* stackEnd;
    uint32_t status = tTaskEnterCritical();

    info->delayTicks = task->wDelayTicks;
    info->prio = task->prio;
    info->slice = task->slice;
    info->state = task->state;
    info->suspendCount = task->suspendCount;

    info->stackFreeSize = 0;
    stackEnd = task->stackBase;
    while ((*stackEnd++ == 0) && (stackEnd <= task->stackBase + task->stackSize / sizeof(tTaskStack))) {
        info->stackFreeSize++;
    }

    info->stackFreeSize *= sizeof(tTaskStack);

    tTaskExitCritical(status);
}
