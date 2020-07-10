#ifndef __TSEM_H
#define __TSEM_H

#include ".\tEvent.h"

typedef struct {
    uint32_t count;
    tEvent event;
    uint32_t maxCount;
}tSem;

typedef struct {
    uint32_t count;
    uint32_t maxCount;
    uint32_t taskCount;
}tSemInfo;

extern void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount);
extern uint32_t tSemWait(tSem* sem, uint32_t waitTicks);
extern uint32_t tSemNoWaitGet(tSem* sem);
extern void tSemNotify(tSem* sem);
uint32_t tSemDestroy(tSem* sem);
void tSemGetInfo(tSem* sem, tSemInfo* info);

#endif
