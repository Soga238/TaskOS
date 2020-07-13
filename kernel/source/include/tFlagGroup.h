#ifndef TFLAG_GROUP_H
#define TFLAG_GROUP_H

#include ".\tEvent.h"


typedef struct {
    tEvent event;
    uint32_t flag;
} tFlagGroup;

#define TFLAGGROUP_CLEAR        (0)
#define TFLAGGROUP_SET          (0x1 << 0)
#define TFLAGGROUP_ANY          (0)
#define TFLAGGROUP_ALL          (0x1 << 1)

#define TFLAGGROUP_SET_ALL          (TFLAGGROUP_SET | TFLAGGROUP_ALL)
#define TFLAGGROUP_SET_ANY          (TFLAGGROUP_SET | TFLAGGROUP_ANY)
#define TFLAGGROUP_CLEAR_ALL        (TFLAGGROUP_CLEAR | TFLAGGROUP_ALL)
#define TFLAGGROUP_CLEAR_ANY        (TFLAGGROUP_CLEAR | TFLAGGROUP_ANY)

#define TFLAGGROUP_CONSUME      (0x1 << 7)

void tFlagGroupInit(tFlagGroup *flagGroup, uint32_t flags);

uint32_t tFlagGroupWait(tFlagGroup *group,
                        uint32_t waitType,
                        uint32_t requestFlag,
                        uint32_t *resultFlag,
                        uint32_t waitTicks
                       );

uint32_t tFlagGroupNoWaitGet(tFlagGroup *group,
                             uint32_t waitType,
                             uint32_t requestFlag,
                             uint32_t *resultFlag
                            );

uint32_t tFlagGroupNotify(tFlagGroup *group,
                          uint8_t isSet,
                          uint32_t flag
                         );

#endif
