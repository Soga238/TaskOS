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

uint32_t idleCount;
uint32_t idleMaxCount;
uint32_t tickCount;

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
    if (NULL != task) {
//        if (task != currentTask) {
            nextTask = task;
            tTaskSwitch();
//        }
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

void tTimeTaskRemove(tTask *task)
{
    // 从延时链表移除任务
    tListRemove(&tTaskDelayedList, &task->delayNode);
}

void tTaskSchedRemove(tTask *task)
{
    tListRemove(&taskTable[task->prio], &task->linkNode);// 从优先级链表删除
    if (tListCount(&taskTable[task->prio]) == 0) {
        tBitmapClear(&taskPrioBitmap, task->prio);  // 该优先级没有任务了，才能把该优先级标志位清0
    }
}


static float cpuUsage;                      // cpu使用率统计
static uint32_t enableCpuUsageStat;         // 是否使能cpu统计

#define TICKS_PER_SEC 1000

static void initCpuUsageStat(void)
{
    idleCount = 0;
    idleMaxCount = 0;
    cpuUsage = 0;
    enableCpuUsageStat = 0;
}

static void checkCpuUsage(void)
{
    // 与空闲任务的cpu统计同步
    if (enableCpuUsageStat == 0) {
        enableCpuUsageStat = 1;
        tickCount = 0;
        return;
    }

    if (tickCount == TICKS_PER_SEC) {
        // 统计最初1s内的最大计数值
        idleMaxCount = idleCount;
        idleCount = 0;

        // 计数完毕，开启调度器，允许切换到其它任务
        tTaskSchedEnable();
    } else if (tickCount % TICKS_PER_SEC == 0) {
        // 之后每隔1s统计一次，同时计算cpu利用率
        cpuUsage = 100 - (idleCount * 100.0 / idleMaxCount);
        idleCount = 0;
    }
}

static void cpuUsageSyncWithSysTick(void)
{
    // 等待与时钟节拍同步
    while (enableCpuUsageStat == 0) {
        ;;
    }
}

void taskIdle(void *argument)
{
    tTaskSchedDisable();

    tInitApp();

    systick_init_1ms();

    cpuUsageSyncWithSysTick();

    while (1) {
        // 不要添加延时，演示OS不完善，会导致bug

        uint32_t status = tTaskEnterCritical();
        idleCount++;
        tTaskExitCritical(status);
    }
}

void tTaskSystemTickHandler(void)
{
    uint32_t status = tTaskEnterCritical();
    tNode *node;
    tTask *task;

    // 所有的延时任务，从优先级链表删除后，都插入到了延时链表中
    int32_t i = 0;
    for (node = tTaskDelayedList.headNode.nextNode;
         i < tListCount(&tTaskDelayedList);
         node = node->nextNode, ++i) {

        task = tNodeParent(node, tTask, delayNode);
        if (--task->wDelayTicks == 0) {
            if (task->waitEvent != NULL) {
                tEventRemoveTask(task, NULL, TIMEOUT);
            }
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

    // 节拍计数增加
    tickCount++;

    // 检查cpu使用率
    checkCpuUsage();

    // 当前调度的模型是：只有最高优先级的任务链表都处于休眠状态，才能执行低优先级的任务
    tTaskSched();
    tTaskExitCritical(status);
}

int main(void)
{
    rcc_init();

    tTaskDelayedListInit();
    tTaskSchedInit();

    initCpuUsageStat();

    tTaskInit(&tTaskIdle, taskIdle, (void *)0, TINYOS_PRIO_COUNT - 1, taskIdleEnv, sizeof(taskIdleEnv));

    nextTask = tTaskHighestReady();

    tTaskRunFirst();

    return 0;
}

