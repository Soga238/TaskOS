#include <string.h>
#include "..\kernel\source\include\TinyOS.h"
#include "..\bsp\rcc\rcc.h"

tTask *currentTask;
tTask *nextTask;

tTask tTask1;
tTask tTask2;

#define TASK1_ENV_SIZE   32
#define TASK2_ENV_SIZE   32

tTaskStack task1Env[TASK1_ENV_SIZE];
tTaskStack task2Env[TASK2_ENV_SIZE];

//uint32_t

tTask *taskTable[2] = {
    &tTask1, &tTask2
};

void tTaskInit(tTask *task, void(*entry)(void *), void *param, tTaskStack *stack)
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
    task->delayTicks = 0;
}

void tTaskSched()
{
    if (currentTask == taskTable[0]) {
        nextTask = taskTable[1];
    } else {
        nextTask = taskTable[0];
    }

    tTaskSwitch();
}

void task1Entry(void *argument)
{
    unsigned long val = (unsigned long)argument;

    systick_init_1ms();

    while (1) {
        val++;
        __nop();
    }
}

void task2Entry(void *argument)
{
    unsigned long val = (unsigned long)argument;
    while (1) {
        val++;
        __nop();
    }
}

int main(void)
{
    rcc_init();

    memset(task1Env, 0xFF, sizeof(task1Env));
    memset(task2Env, 0xFF, sizeof(task2Env));

    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, &task1Env[TASK1_ENV_SIZE]);
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, &task2Env[TASK2_ENV_SIZE]);

    nextTask = taskTable[0];

    tTaskRunFirst();

    while (1) {
        ;
    }

    return 0;
}

void SysTick_Handler(void)
{
    static uint32_t s_wCount = 0;

    // 10MS 定时周期切换任务
    if (++s_wCount % 10 == 0) {
        tTaskSched();
    }
}
