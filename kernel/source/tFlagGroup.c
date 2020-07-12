#include ".\include\TinyOS.h"

void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags)
{
    tEventInit(&flagGroup->event, EVENT_TYPE_FLAG_GROUP);
    flagGroup->flag = flags;
}
