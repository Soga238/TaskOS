#ifndef TMUTEX_H
#define TMUTEX_H

#include ".\tEvent.h"

typedef struct {

    tEvent event;
    
    uint32_t lockCount;     // 锁计数器
    
    tTask *ownerTask;       // 所属任务
    
    uint32_t ownerOriginalPrio; // 任务优先级
} tMutex;

typedef struct {

    uint32_t taskCount;
    uint32_t ownerPrio;
    uint32_t inheritedPrio;

    tTask* owner;
    uint32_t lockedCount;
}tMutexInfo;


void tMutexInit(tMutex *mutex);
uint32_t tMutexWait(tMutex* mutex, uint32_t waitTicks);
uint32_t tMutexNoWaitGet(tMutex* mutex);
uint32_t tMutexNotify(tMutex* mutex);

uint32_t tMutexDestroy(tMutex *mutex);
void tMutexGetInfo(tMutex *mutex, tMutexInfo);

#endif
