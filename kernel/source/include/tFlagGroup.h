#ifndef TFLAG_GROUP_H
#define TFLAG_GROUP_H

#include ".\tEvent.h"


typedef struct {
    tEvent event;
    uint32_t flag;
}tFlagGroup;


void tFlagGroupInit(tFlagGroup *flagGroup, uint32_t flags);

#endif
