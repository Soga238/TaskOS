#ifndef TTIMER_H
#define TTIMER_H

#include ".\tEvent.h"


typedef enum {
    CREATED,
    START,
    RUNNING,
    SETOPED,
    DESTROYED
} tTimerState;


typedef struct {
    tNode linkNode;

    tTimerState state;

    uint32_t startDelayTicks;
    uint32_t delayTicks;
    uint32_t durationTicks;

    void (*timerFunc)(void *arg);

    void *arg;

    uint32_t config;

} tTimer;

void tTimerInit(
    tTimer *timer,
    uint32_t delayTicks,
    uint32_t durationTicks,
    void (*timerFunc)(void *arg),
    void *arg,
    uint32_t config
);

#define TIMER_CONFIG_TYPE_SOFT
#define TIMER_CONFIG_TYPE_HARD

#endif

