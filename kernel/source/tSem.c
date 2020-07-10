#include ".\include\TinyOS.h"


void tSemInit(tSem *sem, uint32_t startCount, uint32_t maxCount)
{
    tEventInit(&sem->event, EVENT_TYPE_SEM);
    sem->maxCount = maxCount;
    if (maxCount == 0) {
        sem->count = startCount;
    } else {
        sem->count = (startCount > maxCount) ? maxCount : startCount;
    }
}

uint32_t tSemWait(tSem *sem, uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();
    uint32_t ret = NO_ERROR;

    // count > 0 收到信号量，可以直接运行后续任务
    if (sem->count > 0) {
        --sem->count;
    } else {
        tEventWait(&sem->event, currentTask, NULL, EVENT_TYPE_SEM, waitTicks);
        tTaskExitCritical(status);
        tTaskSched();   // 直接切换，运行其他任务
        ret = currentTask->waitEventResult;
    }

    tTaskExitCritical(status);
    return ret;
}

uint32_t tSemNoWaitGet(tSem *sem)
{
    uint32_t status = tTaskEnterCritical();
    uint32_t ret = NO_ERROR;

    // count > 0 收到信号量，可以直接运行后续任务
    if (sem->count > 0) {
        --sem->count;
    } else {
        ret = RESOURCE_UNAVALIABLE;
    }

    tTaskExitCritical(status);
    return ret;
}

void tSemNotify(tSem *sem)
{
    uint32_t status = tTaskEnterCritical();

    if (tEventWaitCount(&sem->event) > 0) {
        tTask *task = tEventWakeUp(&sem->event, NULL, NO_ERROR);    // 取出的是 sem->eventList 中的第一个任务
        if (task->prio > currentTask->prio) {
            // 取出的任务优先级比当前高，需要切换到高优先级任务运行
            tTaskSched();
        } else {

            // TODO: 未加入maxCount = 0 时的逻辑处理
            sem->count++;
            if (sem->maxCount != 0 && sem->count > sem->maxCount) {
                sem->count = sem->maxCount;
            }
        }
    }

    tTaskExitCritical(status);
}
