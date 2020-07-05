#ifndef TEVENT_H
#define TEVENT_H

#include ".\tTask.h"

typedef enum {
    EVENT_TYPE_UNKNOWN,
}tEventType;

typedef struct _tEvent {
    tEventType type;
    tList waitList;
}tEvent;

void tEventInit(tEvent* event, tEventType type);
void tEventWait(tEvent* event, tTask* task, void* msg, uint32_t state, uint32_t timeout);
tTask* tEventWakeUp(tEvent* event, void* msg, uint32_t result);
void tEventRemoveTask(tTask* task, void* msg, uint32_t result);

#endif // !TEVENT_H