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

#define TASK1_ENV_SIZE   64
#define TASK2_ENV_SIZE   64
#define TASK3_ENV_SIZE   64
#define TASK4_ENV_SIZE   64

tTaskStack task1Env[TASK1_ENV_SIZE];
tTaskStack task2Env[TASK2_ENV_SIZE];
tTaskStack task3Env[TASK3_ENV_SIZE];
tTaskStack task4Env[TASK4_ENV_SIZE];

tSem sem1;
tSem sem2;

void task1DestroyFunc(void* param)
{
    taskFlag1 = 0;
}


void task1Entry(void *argument)
{
    systick_init_1ms();

    // 设备任务释放回调函数
    tTaskSetCleanCallFunc(currentTask, task1DestroyFunc, NULL);

    tSemInit(&sem1, 0, 10);

    while (1) {

        tSemWait(&sem1, 10);

        taskFlag1 = 0;
        tTaskDelay(1);
        taskFlag1 = 1;
        tTaskDelay(1);
    }
}

#define DELAY() do{for(int32_t i = 0; i < 0xFF; i++){}}while(0)

void task2Entry(void *argument)
{
    volatile int error = 0;

    while (1) {

        taskFlag2 = 0;
        tTaskDelay(1);
        taskFlag2 = 1;
        tTaskDelay(1);

        tSemNotify(&sem1);
        error = tSemNoWaitGet(&sem1);
        error = tSemNoWaitGet(&sem1);

    }
}

void task3Entry(void *argument)
{
     tSemInit(&sem2, 0, 10);
    while (1) {
        tSemWait(&sem2, 10);    // 等待超时运行
        
        taskFlag3 = 0;
        tTaskDelay(1);
        taskFlag3 = 1;
        tTaskDelay(1);
    }
}

void task4Entry(void *argument)
{
    while (1) {
        
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
    tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 1, &task3Env[TASK3_ENV_SIZE]);
    tTaskInit(&tTask4, task4Entry, (void *)0x44444444, 1, &task4Env[TASK4_ENV_SIZE]);

}

