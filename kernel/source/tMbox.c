#include ".\include\TinyOS.h"


void tMBoxInit(tMBox *box, void **msgBuffer, uint32_t maxCount)
{
    tEventInit(&box->event, EVENT_TYPE_MAILBOX);

    box->msgBuffer = msgBuffer;
    box->maxCount = maxCount;
    box->count = 0;
    box->read = 0;
    box->write = 0;
}

uint32_t tMboxWait(tMBox *box, void **msg, uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();

    if (box->count > 0) {
        --box->count;
        // 取出消息
        *msg = box->msgBuffer[box->read];
        if (++box->read, box->read >= box->maxCount) {
            box->read = 0;
        }

        tTaskExitCritical(status);
        return NO_ERROR;
    } else {
        tEventWait(&box->event, currentTask, msg, EVENT_TYPE_MAILBOX, waitTicks);
        tTaskExitCritical(status);

        // 切换任务，等到事件触发或者超时触发
        tTaskSched();

        *msg = currentTask->eventMsg;
        return currentTask->waitEventResult;
    }

}

uint32_t tMboxNoWaitGet(tMBox *box, void **msg)
{
    uint32_t status = tTaskEnterCritical();

    if (box->count > 0) {
        // 取出消息
        *msg = box->msgBuffer[box->read];
        if (++box->read, box->read >= box->maxCount) {
            box->read = 0;
        }

        tTaskExitCritical(status);
        return NO_ERROR;
    }

    tTaskExitCritical(status);
    return RESOURCE_UNAVALIABLE;
}

uint32_t tMboxNotify(tMBox *box, void *msg, uint32_t notifyOption)
{
    uint32_t status = tTaskEnterCritical();


    if (tEventWaitCount(&box->event) > 0) { // 有任务在等待
        tTask *task = tEventWakeUp(&box->event, msg, NO_ERROR);
        if (task->prio > currentTask->prio) {
            tTaskSched();
        }

    } else {
        if (box->count >= box->maxCount) {
            tTaskExitCritical(status);
            return RESOURCE_FULL;
        }

        if (notifyOption & MBOX_SEND_FRONT) { // 插到头部
            if (box->read <= 0) {
                box->read = box->maxCount - 1;
            } else {
                box->read--;
            }

            box->msgBuffer[box->read] = msg;

        } else {
            box->msgBuffer[box->write++] = msg;
            if (box->write >= box->maxCount) {
                box->write = 0;
            }
        }

        box->count++;

    }

    tTaskExitCritical(status);
    return NO_ERROR;

}

void tMboxFlush(tMBox *mbox)
{
    uint32_t status = tTaskEnterCritical();

    if (tEventWaitCount(&mbox->event) == 0) {   // 没有任务
        mbox->read = 0;
        mbox->write = 0;
        mbox->count = 0;
    }

    tTaskExitCritical(status);
}

uint32_t tMboxDestroy(tMBox *mbox)
{
    uint32_t status = tTaskEnterCritical();
    uint32_t count = tEventRemoveAll(&mbox->event, NULL, DELETE);

    if (count > 0) {
        tTaskSched();
    }

    tTaskExitCritical(status);
    return count;
}


