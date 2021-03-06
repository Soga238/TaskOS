#ifndef TTIMER_H
#define TTIMER_H

#include ".\tEvent.h"


typedef enum {
    CREATED,
    START,
    RUNNING,
    STEOPED,
    DESTROYED
} tTimerState;


typedef struct {
    tNode linkNode;

    tTimerState state;

    uint32_t startDelayTicks;   // 初次启动延后的ticks数
    uint32_t delayTicks;        // 当前定时递减计数值
    uint32_t durationTicks;     // 周期定时时的周期tick数

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

#define TIMER_CONFIG_TYPE_SOFT  0x01
#define TIMER_CONFIG_TYPE_HARD  0x02

void tTimerModuleInit(void);

#endif

