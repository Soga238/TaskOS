#include ".\include\TinyOS.h"

void tTimerInit(tTimer *timer,
                uint32_t delayTicks,
                uint32_t durationTicks,
                void (*timerFunc)(void *arg),
                void *arg,
                uint32_t config
               )
{

    timer->arg = arg;
    timer->config = config;

    timer->startDelayTicks = delayTicks;
    timer->durationTicks = durationTicks;

    timer->state = CREATED;

    timer->timerFunc = timerFunc;

    tNodeInit(&timer->linkNode);

    if (delayTicks == 0) {
        timer->delayTicks = durationTicks;
    } else {
        timer->delayTicks = timer->startDelayTicks;
    }

}


