#include <string.h>
#include "..\kernel\source\include\TinyOS.h"
#include "..\bsp\rcc\rcc.h"

tTask *currentTask;
tTask *nextTask;

tTask tTask1;
tTask tTask2;
tTask tTaskIdle;

#define TASK1_ENV_SIZE   32
#define TASK2_ENV_SIZE   32
#define TASK_IDLE_ENV_SIZE  32

tTaskStack task1Env[TASK1_ENV_SIZE];
tTaskStack task2Env[TASK2_ENV_SIZE];
tTaskStack taskIdleEnv[TASK_IDLE_ENV_SIZE];

tTask *taskTable[TINYOS_PRIO_COUNT] = {
    &tTask1, &tTask2
};

tBitmap taskPrioBitmap;
uint8_t schedLockCount;
tList tTaskDelayedList; // 延时链表

void tTaskDelay(uint32_t wTicks);

void tTaskInit(tTask *task, void(*entry)(void *), void *param, uint32_t prio, tTaskStack *stack)
{
    *(--stack) = (unsigned long)(1 << 24);              // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
    *(--stack) = (unsigned long)entry;                  // 程序的入口地址
    *(--stack) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--stack) = (unsigned long)0x12;                   // R12, 未用
    *(--stack) = (unsigned long)0x3;                    // R3, 未用
    *(--stack) = (unsigned long)0x2;                    // R2, 未用
    *(--stack) = (unsigned long)0x1;                    // R1, 未用
    *(--stack) = (unsigned long)param;                  // R0 = param, 传给任务的入口函数
    *(--stack) = (unsigned long)0x11;                   // R11, 未用
    *(--stack) = (unsigned long)0x10;                   // R10, 未用
    *(--stack) = (unsigned long)0x9;                    // R9, 未用
    *(--stack) = (unsigned long)0x8;                    // R8, 未用
    *(--stack) = (unsigned long)0x7;                    // R7, 未用
    *(--stack) = (unsigned long)0x6;                    // R6, 未用
    *(--stack) = (unsigned long)0x5;                    // R5, 未用
    *(--stack) = (unsigned long)0x4;                    // R4, 未用

    task->stack = stack;                                // 保存最终的值
    task->wDelayTicks = 0;
    task->prio = prio;

    task->state = TINYOS_TASK_STATE_RDY;
    tNodeInit(&task->delayNode);

    taskTable[prio] = task;
    tBitmapSet(&taskPrioBitmap, prio);
}

// 返回优先级最好的就绪任务块指针
tTask *tTaskHighestReady(void)
{
    uint32_t prio = tBitmapGetFirstSet(&taskPrioBitmap);
    return taskTable[prio];
}

void tTaskDelayedListInit()
{
    tListInit(&tTaskDelayedList);
}

void tTimeTaskWait(tTask *task, uint32_t ticks)
{
    // 插入延时队列
    task->wDelayTicks = ticks;
    tListAddLast(&tTaskDelayedList, &task->delayNode);
    task->state |= TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskWakeUp(tTask *task)
{
    // 从延时队列移除任务
    tListRemove(&tTaskDelayedList, &task->delayNode);
    task->state &= ~TINYOS_TASK_STATE_RDY;
}

void tTaskScedRdy(tTask *task)
{
    taskTable[task->prio] = task;
    tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskScedUnRdy(tTask *task)
{
    taskTable[task->prio] = NULL;
    tBitmapClear(&taskPrioBitmap, task->prio);
}

void tTaskSched()
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

void task1Entry(void *argument)
{
    systick_init_1ms();

    while (1) {
        tTaskDelay(1);
        __nop();
        tTaskDelay(1);
        __nop();
    }
}

void task2Entry(void *argument)
{
    while (1) {
        tTaskDelay(1);
        __nop();
        tTaskDelay(1);
        __nop();
    }
}

void taskIdle(void *argument)
{
    while (1) {
        tTaskDelay(1);
        __nop();
        tTaskDelay(1);
        __nop();
    }
}

void tTaskSystemTickHandler(void)
{
    uint32_t status = tTaskEnterCritical();
    tNode *node;
    tTask* task;

    for (node = tTaskDelayedList.headNode.nextNode;
         node != &(tTaskDelayedList.headNode);
         node = node->nextNode) {

        task = tNodeParent(node, tTask, delayNode);
        if (--task->wDelayTicks == 0) {
            tTimeTaskWakeUp(task);// 将task任务从延时队列删除
            tTaskScedRdy(task);// 插入到就绪任务表中
        }
    }

    tTaskSched();
    tTaskExitCritical(status);
}

void tTaskDelay(uint32_t wTicks)
{
    uint32_t status = tTaskEnterCritical();

    tTimeTaskWait(currentTask, wTicks); // 将当前任务插入延时队列
    tTaskScedUnRdy(currentTask); // 从就绪任务表中删除当前任务

    tTaskSched();

    tTaskExitCritical(status);
}

int main(void)
{
    rcc_init();

    memset(task1Env, 0xFF, sizeof(task1Env));
    memset(task2Env, 0xFF, sizeof(task2Env));

    tListInit(&tTaskDelayedList);

    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[TASK1_ENV_SIZE]);
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[TASK2_ENV_SIZE]);
    tTaskInit(&tTaskIdle, taskIdle, (void *)0, TINYOS_PRIO_COUNT - 1, &taskIdleEnv[TASK_IDLE_ENV_SIZE]);

    nextTask = tTaskHighestReady();

    tTaskRunFirst();

//    while (1) {
//        ;
//    }

    return 0;
}

void SysTick_Handler(void)
{
    static uint32_t s_wCount = 0;

    // 10MS 定时周期切换任务
    if (++s_wCount % 10 == 0) {
        tTaskSystemTickHandler();
    }
}
