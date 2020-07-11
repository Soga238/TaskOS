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

typedef uint8_t(*tBlock)[100];
uint8_t mem1[20][100];
tMemBlock memBlock1;

void task1DestroyFunc(void* param)
{
    taskFlag1 = 0;
}


void task1Entry(void *argument)
{
    systick_init_1ms();

    tBlock block[20];

    tMemBlockInit(&memBlock1, (uint8_t *)mem1, 100, 20);

    for (int32_t i = 0; i < 20; i++) {
        tMemBlockWait(&memBlock1, &block[i], 0);
    }

    // tTaskDelay(1);

    for (int32_t i = 0; i < 20; i++) {
        memset(block[i], i, 100);
        tMemBlockNotify(&memBlock1, block[i]);
        // tTaskDelay(2);
    }

    while (1) {

        taskFlag1 = 0;
        tTaskDelay(1);
        taskFlag1 = 1;
        tTaskDelay(1);
    }
}

#define DELAY() do{for(int32_t i = 0; i < 0xFF; i++){}}while(0)

void task2Entry(void *argument)
{
    void *ptr = 0;
    
    while (1) {
        taskFlag2 = 0;
        tTaskDelay(1);
        
        tMemBlockWait(&memBlock1, &ptr, 10);
        
        taskFlag2 = 1;
        tTaskDelay(1);
    }
}

void task3Entry(void *argument)
{
    while (1) {

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

