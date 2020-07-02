#include <string.h>
#include "..\kernel\source\include\TinyOS.h"
#include "..\bsp\rcc\rcc.h"

tTask *currentTask;
tTask *nextTask;
tTask tTaskIdle;
tTaskStack taskIdleEnv[TASK_IDLE_ENV_SIZE];

/*
    taskTable 每个优先级都有一个任务链表

    ++++++++++++
        list    prio = 0
    ++++++++++++
        list    prio = 1
    +++++++++++
        ...
    +++++++++++
        list    prio = 31
    +++++++++++

*/
tList taskTable[TINYOS_PRIO_COUNT];

tBitmap taskPrioBitmap;
uint8_t schedLockCount;
tList tTaskDelayedList; // 延时链表

// 返回优先级最好的就绪任务块指针
tTask *tTaskHighestReady(void)
{
    uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
    tNode *node = tListFirst(&taskTable[highestPrio]);  // 从最高优先级链表中取出首个节点
    return tNodeParent(node, tTask, linkNode);  // 将node节点转换成任务指针
}

void tTaskDelayedListInit()
{
    tListInit(&tTaskDelayedList);
}

void tTimeTaskWait(tTask *task, uint32_t ticks)
{
    // 插入延时链表
    task->wDelayTicks = ticks;
    tListAddLast(&tTaskDelayedList, &task->delayNode);
    task->state |= TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskWakeUp(tTask *task)
{
    // 从延时链表移除任务
    tListRemove(&tTaskDelayedList, &task->delayNode);
    task->state &= ~TINYOS_TASK_STATE_RDY;
}

void tTaskScedRdy(tTask *task)
{
    tListAddFirst(&taskTable[task->prio], &task->linkNode); // 插入有优先级链表
    tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskScedUnRdy(tTask *task)
{
    tListRemove(&taskTable[task->prio], &task->linkNode);// 从优先级链表删除
    if (tListCount(&taskTable[task->prio]) == 0) {
        tBitmapClear(&taskPrioBitmap, task->prio);  // 该优先级没有任务了，才能把该优先级标志位清0
    }
}

void tTaskSched(void)
{
    uint32_t status = tTaskEnterCritical();

    if (schedLockCount > 0) {   // 判断上锁退出任务
        tTaskExitCritical(status);
        return;
    }

    // 不是当前任务才切换
    // NOTE: 视频代码bug，未判断空指针
    tTask *task = tTaskHighestReady();
    if (NULL != task && task != currentTask) {
        nextTask = task;
        tTaskSwitch();
    }

    tTaskExitCritical(status);
}

void tTaskSchedInit(void)
{
    schedLockCount = 0;
    tBitmapInit(&taskPrioBitmap);

    for (int32_t i = 0; i < TINYOS_PRIO_COUNT; i++) {
        tListInit(&taskTable[i]);   // 初始化每个优先级的对应链表
    }
}

void tTaskSchedDisable(void)
{
    uint32_t status = tTaskEnterCritical();

    if (schedLockCount < 255) {
        schedLockCount++;
    }

    tTaskExitCritical(status);
}

void tTaskSchedEnable(void)
{
    uint32_t status = tTaskEnterCritical();

    if (schedLockCount > 0) {
        if (--schedLockCount == 0) {
            tTaskSched();   // 切换任务
        }
    }

    tTaskExitCritical(status);
}


void taskIdle(void *argument)
{
    while (1) {
        // 不要添加延时，演示OS不完善，会导致bug
    }
}

void tTaskSystemTickHandler(void)
{
    uint32_t status = tTaskEnterCritical();
    tNode *node;
    tTask *task;

    // 所有的延时任务，从优先级链表删除后，都插入到了延时链表中
    for (node = tTaskDelayedList.headNode.nextNode;
         node != &(tTaskDelayedList.headNode);
         node = node->nextNode) {

        task = tNodeParent(node, tTask, delayNode);
        if (--task->wDelayTicks == 0) {
            tTimeTaskWakeUp(task);// 将task任务从延时链表删除
            tTaskScedRdy(task);// 插入到就绪任务表中
        }
    }

    // 某个优先级下，首个任务时间片减为0后，第二个任务递进到首位，释放的任务排到末尾
    if (--currentTask->slice == 0) {
        if (0 < tListCount(&taskTable[currentTask->prio])) {
            tListRemoveFirst(&taskTable[currentTask->prio]);
            tListAddLast(&taskTable[currentTask->prio], &currentTask->linkNode);
            currentTask->slice = TINYOS_SLICE_MAX;
        }
    }

    // 当前调度的模型是：只有最高优先级的任务链表都处于休眠状态，才能执行低优先级的任务
    tTaskSched();
    tTaskExitCritical(status);
}



int main(void)
{
    rcc_init();

    tTaskDelayedListInit();
    tTaskSchedInit();

    tInitApp();
    
    tTaskInit(&tTaskIdle, taskIdle, (void *)0, TINYOS_PRIO_COUNT - 1, &taskIdleEnv[TASK_IDLE_ENV_SIZE]);

    nextTask = tTaskHighestReady();

    tTaskRunFirst();

    return 0;
}

