#include ".\include\TinyOS.h"

static tList tTimerHardList;
static tList tTimerSoftList;

static tSem tTimerProtectSem;
static tSem tTimerTickSem;

#define TINYOS_TIMERTASK_PRIO   6
#if TINYOS_TIMERTASK_PRIO >= TINYOS_PRIO_COUNT - 1
#error "TINYOS_TIMERTASK_PRIO >= TINYOS_PRIO_COUNT - 1"
#endif // TINYOS_TIMERTASK_PRIO >= TINYOS_PRIO_COUNT - 1


#define TIMER_TASK_STACKE_SIZE  256

static tTask tTimeTask;
static tTaskStack taskEnv[TIMER_TASK_STACKE_SIZE];

void tTimerInit(tTimer *timer,
                uint32_t delayTicks,
                uint32_t durationTicks,
                void (*timerFunc)(void *arg),
                void *arg,
                uint32_t config
               )
{

    tNodeInit(&timer->linkNode);

    timer->arg = arg;
    timer->config = config;
    timer->startDelayTicks = delayTicks;
    timer->durationTicks = durationTicks;
    timer->state = CREATED;
    timer->timerFunc = timerFunc;
    if (delayTicks == 0) {
        timer->delayTicks = durationTicks;
    } else {
        timer->delayTicks = timer->startDelayTicks;
    }
}

static void tTimerSoftTask(void *arg)
{
    for (;;) {
        tSemWait(&tTimerTickSem, 0);

        tSemWait(&tTimerProtectSem, 0);
    }
}

void tTimerModuleInit(void)
{
    tListInit(&tTimerHardList);
    tListInit(&tTimerSoftList);

    tSemInit(&tTimerProtectSem, 1, 1);
    tSemInit(&tTimerTickSem, 0, 0);

    tTaskInit(&tTimeTask, tTimerSoftTask, (void *)0x22222222, TINYOS_TIMERTASK_PRIO, &taskEnv[TIMER_TASK_STACKE_SIZE]);
}

void tTimerStart(tTimer *timer)
{
    switch (timer->state) {
        case CREATED:
        case STEOPED:
            timer->delayTicks = timer->startDelayTicks ? timer->startDelayTicks : timer->durationTicks;
            timer->state = START;
            if (timer->config & TIMER_CONFIG_TYPE_HARD) {
                uint32_t status = tTaskEnterCritical();
                tListAddFirst(&tTimerHardList, &timer->linkNode);
                tTaskExitCritical(status);
            } else {
                tSemWait(&tTimerProtectSem, 0);
                tListAddFirst(&tTimerSoftList, &timer->linkNode);
                tSemNotify(&tTimerProtectSem);
            }
        default:
            break;
    }
}

void tTimerStop(tTimer *timer)
{
    switch (timer->state) {
        case START:
        case RUNNING:
            if (timer->config & TIMER_CONFIG_TYPE_HARD) {
                uint32_t status = tTaskEnterCritical();
                tListRemove(&tTimerHardList, &timer->linkNode);
                tTaskExitCritical(status);
            } else {
                tSemWait(&tTimerProtectSem, 0);
                tListRemove(&tTimerSoftList, &timer->linkNode);
                tSemNotify(&tTimerProtectSem);
            }
        default:
            break;
    }
}
