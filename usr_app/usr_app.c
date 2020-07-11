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


tMBox mbox1;
tMBox mbox2;
void* mbox1Buffer[20];
void* mbox2Buffer[20];

uint32_t g_msg[20];

void task1DestroyFunc(void* param)
{
    taskFlag1 = 0;
}


void task1Entry(void *argument)
{
    systick_init_1ms();

    tMBoxInit(&mbox1, mbox1Buffer, 20);

    while (1) {

        for (int32_t i = 0; i < 20; i++) {
            g_msg[i] = i;
            tMboxNotify(&mbox1, &g_msg[i], MBOX_SEND_NORMAL);
        }

        tTaskDelay(100);

        for (int32_t i = 0; i < 20; i++) {
            g_msg[i] = i;
            tMboxNotify(&mbox1, &g_msg[i], MBOX_SEND_FRONT);
        }
        
        tTaskDelay(100);

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

        void* msg;
        uint32_t err = tMboxWait(&mbox1, &msg, 0);
        if (err == NO_ERROR) {
            volatile uint32_t value = *(uint32_t*)msg;
            taskFlag2 = value;
            tTaskDelay(1);
        }
    }
}

void task3Entry(void *argument)
{
    tMBoxInit(&mbox2, mbox2Buffer, 20);
    while (1) {

        void* msg;
        uint32_t err = tMboxWait(&mbox2, &msg, 100);

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

