#include <string.h>

#include "..\kernel\source\include\TinyOS.h"
#include "..\bsp\rcc\rcc.h"

tTask tTask1;
tTask tTask2;
tTask tTask3;
tTask tTask4;

int32_t taskFlag1;
int32_t taskFlag2;
int32_t taskFlag3;
int32_t taskFlag4;

#define TASK1_ENV_SIZE   32
#define TASK2_ENV_SIZE   32
#define TASK3_ENV_SIZE   32
#define TASK4_ENV_SIZE   32

tTaskStack task1Env[TASK1_ENV_SIZE];
tTaskStack task2Env[TASK2_ENV_SIZE];
tTaskStack task3Env[TASK3_ENV_SIZE];
tTaskStack task4Env[TASK4_ENV_SIZE];

tEvent eventWaitTimeout;
tEvent eventWaitNormal;

void task1DestroyFunc(void* param)
{
    taskFlag1 = 0;
}


void task1Entry(void *argument)
{
    systick_init_1ms();

    // 设备任务释放回调函数
    tTaskSetCleanCallFunc(currentTask, task1DestroyFunc, NULL);

    tEventInit(&eventWaitTimeout, EVENT_TYPE_UNKNOWN);

    while (1) {

        tEventWait(&eventWaitTimeout, currentTask, NULL, 0, 5);
        tTaskSched();   // 调度下任务

        taskFlag1 = 0;
        tTaskDelay(1);
        taskFlag1 = 1;
        tTaskDelay(1);
    }
}

#define DELAY() do{for(int32_t i = 0; i < 0xFF; i++){}}while(0)

void task2Entry(void *argument)
{
    while (1) {
        tEventWait(&eventWaitNormal, currentTask, NULL, 0, 0);
        tTaskSched();   // 调度下任务

        taskFlag2 = 0;
        tTaskDelay(1);
        taskFlag2 = 1;
        tTaskDelay(1);

    }
}

void task3Entry(void *argument)
{
    tEventInit(&eventWaitNormal, EVENT_TYPE_UNKNOWN);

    while (1) {

        tEventWait(&eventWaitNormal, currentTask, NULL, 0, 0);
        tTaskSched();   // 调度下任务

        taskFlag3 = 0;
        tTaskDelay(1);
        taskFlag3 = 1;
        tTaskDelay(1);
    }
}

void task4Entry(void *argument)
{
    int task3Deleted = 0;

    while (1) {

        tTask* task = tEventWakeUp(&eventWaitNormal, NULL, 0);
        tTaskSched();   // 调度下任务

        taskFlag4 = 0;
        tTaskDelay(1);
        taskFlag4 = 1;
        tTaskDelay(1);

    }
}

void tInitApp(void)
{
    memset(task1Env, 0xFF, sizeof(task1Env));
    memset(task2Env, 0xFF, sizeof(task2Env));
    memset(task3Env, 0xFF, sizeof(task3Env));

    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[TASK1_ENV_SIZE]);
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[TASK2_ENV_SIZE]);
    tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 0, &task3Env[TASK3_ENV_SIZE]);
    tTaskInit(&tTask4, task4Entry, (void *)0x44444444, 1, &task4Env[TASK4_ENV_SIZE]);

}

