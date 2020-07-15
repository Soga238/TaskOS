#include ".\include\TinyOS.h"

void tMutexInit(tMutex *mutex)
{
    mutex->lockCount = 0;
    mutex->ownerOriginalPrio = 0;
    mutex->ownerTask = NULL;

    tEventInit(&mutex->event, EVENT_TYPE_MUTEX);
}

uint32_t tMutexWait(tMutex *mutex, uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();

    if (mutex->lockCount <= 0) {
        // 资源锁没有被占用
        mutex->lockCount++;
        mutex->ownerTask = currentTask;
        mutex->ownerOriginalPrio = currentTask->prio;   // 继承任务的优先级

        tTaskExitCritical(status);
        return NO_ERROR;
    } else {

        if (mutex->ownerTask == currentTask) {  // 被自身嵌套调用
            mutex->lockCount++; // 自增加锁计数器后，直接退出函数
            tTaskExitCritical(status);
            return NO_ERROR;
        }

        // 注意：prio数值越低，优先级越高
        // 当前任务优先级比owner任务优先极高
        if (currentTask->prio < mutex->ownerOriginalPrio) {
            tTask *owner = mutex->ownerTask;
            if (owner->state == TINYOS_TASK_STATE_RDY) {

                // 提升低任务优先级
                tTaskScedUnRdy(owner);
                owner->prio = currentTask->prio;
                tTaskScedRdy(owner);
            } else {
                // 其它状态，只需要修改优先级
                owner->prio = currentTask->prio;
            }

        }

        tEventWait(&mutex->event, currentTask, NULL, EVENT_TYPE_MUTEX, waitTicks);
        tTaskExitCritical(status);

        tTaskSched();
        return currentTask->waitEventResult;
    }
}


uint32_t tMutexNoWaitGet(tMutex *mutex)
{
    uint32_t status = tTaskEnterCritical();

    if (mutex->lockCount <= 0) {
        mutex->lockCount++;
        mutex->ownerTask = currentTask;
        mutex->ownerOriginalPrio = currentTask->prio;   // 继承任务的优先级

        tTaskExitCritical(status);
        return NO_ERROR;
    } else {

        if (mutex->ownerTask == currentTask) {
            mutex->lockCount++;
            tTaskExitCritical(status);
            return NO_ERROR;
        }

        tTaskExitCritical(status);
        return RESOURCE_UNAVALIABLE;
    }
}

uint32_t tMutexNotify(tMutex *mutex)
{
    uint32_t status = tTaskEnterCritical();

    // 锁定计数0，信号量未被锁定，直接退出
    if (mutex->lockCount <= 0) {
        tTaskExitCritical(status);
        return NO_ERROR;
    }

    // 互斥量应该被使用者释放
    if (mutex->ownerTask != currentTask) {
        tTaskExitCritical(status);
        return ERROR_OWNER;
    }

    if (--mutex->lockCount, mutex->lockCount > 0) {   // 减一计数后仍不为0，直接退出
        tTaskExitCritical(status);
        return NO_ERROR;
    }

    // 优先级不等，发生了优先级继承
    if (mutex->ownerOriginalPrio != mutex->ownerTask->prio) {
        if (mutex->ownerTask->state == TINYOS_TASK_STATE_RDY) {

            // 恢复最初的优先级
            tTaskScedUnRdy(mutex->ownerTask);
            currentTask->prio = mutex->ownerOriginalPrio;
            tTaskScedRdy(mutex->ownerTask);
        } else {
            currentTask->prio = mutex->ownerOriginalPrio;
        }
    }
     
    // 锁释放完毕后，检查链表中是否有任务在等待锁资源
    if (tEventWaitCount(&mutex->event)) {
        tTask* task = tEventWakeUp(&mutex->event, 0, NO_ERROR); // 直接唤醒头部任务，不是很合理

        mutex->ownerTask = task;
        mutex->ownerOriginalPrio = task->prio;
        mutex->lockCount++;

        // task优先级高，请求一次任务调度
        if (task->prio < currentTask->prio) {
            tTaskSched();
        }

    }

    tTaskExitCritical(status);
    return NO_ERROR;
}


