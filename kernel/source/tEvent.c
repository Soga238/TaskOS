#include ".\include\TinyOS.h"

void tEventInit(tEvent *event, tEventType type)
{
    event->type = EVENT_TYPE_UNKNOWN;
    tListInit(&event->waitList);
}

void tEventWait(tEvent *event, tTask *task, void *msg, uint32_t state, uint32_t timeout)
{
    uint32_t status = tTaskEnterCritical();
    
    task->state |= state;
    task->waitEvent = event;
    task->eventMsg = msg;
    task->waitEventResult = NO_ERROR;

    // 挂起任务
    tTaskScedUnRdy(task);

    // 插入到事件链表中
    tListAddLast(&event->waitList, &task->linkNode);

    if (timeout) {
        tTimeTaskWait(task, timeout);   // 插入到延时链表中，指定时间内没有接收到事件，就触发超时
    }
    
    tTaskExitCritical(status);
}

tTask* tEventWakeUp(tEvent* event, void* msg, uint32_t result)
{
    uint32_t status = tTaskEnterCritical();
    tNode* node;
    tTask* task;

    node = tListRemoveFirst(&event->waitList);  // 取出事件链表中的第一个等待任务
    if (node != NULL) {
        task = tNodeParent(node, tTask, linkNode);  // 计算出任务对象
        task->waitEvent = NULL;
        task->waitEventResult = result;
        task->eventMsg = msg;
        task->state &= ~TINYOS_WAIT_MASK;

        if (task->wDelayTicks != 0) {
            tTimeTaskWakeUp(task);  // 从延时任务链表删除
        }

        tTaskScedRdy(task); // 挂起任务插回优先级任务就绪链表
    }

    tTaskExitCritical(status);

    return task;
}

void tEventRemoveTask(tTask* task, void* msg, uint32_t result)
{
    uint32_t status = tTaskEnterCritical();

    tListRemove(&task->waitEvent->waitList, &task->linkNode);
    task->waitEvent = NULL;
    
    task->waitEventResult = result;
    task->eventMsg = msg;
    task->state &= ~TINYOS_WAIT_MASK;

    tTaskExitCritical(status);
}