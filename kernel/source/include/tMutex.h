#ifndef TMUTEX_H
#define TMUTEX_H

#include ".\tEvent.h"

typedef struct {

    tEvent event;
    
    uint32_t lockCount;     // 锁计数器
    
    tTask *ownerTask;       // 所属任务
    
    uint32_t ownerOriginalPrio; // 任务优先级
} tMutex;


void tMutexInit(tMutex *mutex);

#endif
