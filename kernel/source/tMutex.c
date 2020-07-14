#include ".\include\TinyOS.h"

void tMutexInit(tMutex* mutex)
{
    mutex->lockCount = 0;
    mutex->ownerOriginalPrio = 0;
    mutex->ownerTask = NULL;

    tEventInit(&mutex->event, EVENT_TYPE_MUTEX);
}


