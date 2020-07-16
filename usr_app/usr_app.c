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

#define TASK1_ENV_SIZE   256
#define TASK2_ENV_SIZE   256
#define TASK3_ENV_SIZE   256
#define TASK4_ENV_SIZE   256

tTaskStack task1Env[TASK1_ENV_SIZE];
tTaskStack task2Env[TASK2_ENV_SIZE];
tTaskStack task3Env[TASK3_ENV_SIZE];
tTaskStack task4Env[TASK4_ENV_SIZE];

tTaskInfo info1;
tTaskInfo info2;
tTaskInfo info3;
tTaskInfo info4;


void task1DestroyFunc(void *param)
{
    taskFlag1 = 0;
}

void task1Entry(void *argument)
{
    systick_init_1ms();

    while (1) {
        taskFlag1 = 0;
        tTaskDelay(1);
        tTaskGetInfo(currentTask, &info1);
        taskFlag1 = 1;
        tTaskDelay(1);    
    }
}

#define DELAY() do{for(int32_t i = 0; i < 0xFF; i++){}}while(0)

void task2Entry(void *argument)
{
    while (1) {
        taskFlag2 = 0;
        tTaskDelay(1);
        tTaskGetInfo(currentTask, &info2);
        taskFlag2 = 1;
        tTaskDelay(1);
    }
}

void task3Entry(void *argument)
{
    while (1) {

        taskFlag3 = 0;
        tTaskDelay(1);
        uint8_t x[6] = {0x01, 2, 3, 4, 5,6};
       
        tTaskGetInfo(currentTask, &info3);
        taskFlag3 = 1;
        tTaskDelay(1);
    }
}

void task4Entry(void *argument)
{
    while (1) {

        taskFlag4 = 0;
        tTaskDelay(1);
        tTaskGetInfo(currentTask, &info4);
        taskFlag4 = 1;
        tTaskDelay(1);
    }
}

void tInitApp(void)
{
    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, task1Env, sizeof(task1Env));
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, task2Env, sizeof(task2Env));
    tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 1, task3Env, sizeof(task3Env));
    tTaskInit(&tTask4, task4Entry, (void *)0x44444444, 1, task4Env, sizeof(task4Env));

}

