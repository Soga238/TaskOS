#include <string.h>

#include "..\kernel\source\include\TinyOS.h"
#include "..\bsp\rcc\rcc.h"

tTask tTask1;
tTask tTask2;
tTask tTask3;

int32_t taskFlag1;
int32_t taskFlag2;
int32_t taskFlag3;

#define TASK1_ENV_SIZE   32
#define TASK2_ENV_SIZE   32
#define TASK3_ENV_SIZE   32

tTaskStack task1Env[TASK1_ENV_SIZE];
tTaskStack task2Env[TASK2_ENV_SIZE];
tTaskStack task3Env[TASK3_ENV_SIZE];

void task1Entry(void *argument)
{
    systick_init_1ms();

    while (1) {
        taskFlag1 = 1;
        tTaskSuspend(currentTask);
        taskFlag1 = 0;
        tTaskSuspend(currentTask);
    }
}

#define DELAY() do{for(int32_t i = 0; i < 0xFF; i++){}}while(0)

void task2Entry(void *argument)
{
    while (1) {
        taskFlag2 = 1;
        tTaskDelay(1);
        tTaskWakeUp(&tTask1);
        taskFlag2 = 0;
        tTaskDelay(1);
        tTaskWakeUp(&tTask1);
    }
}

void task3Entry(void *argument)
{
    while (1) {
        taskFlag3 = 1;
        tTaskDelay(1);
        taskFlag3 = 0;
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
}

