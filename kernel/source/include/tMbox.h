#ifndef TMOBX_H
#define TMOBX_H

#include "tEvent.h"

#define MBOX_SEND_NORMAL    0
#define MBOX_SEND_FRONT     1

typedef struct {
    tEvent event;
    uint32_t count;     // 消息数
    uint32_t read;      // 读索引
    uint32_t write;     // 写索引
    uint32_t maxCount;
    void** msgBuffer;   // 消息缓冲区头指针
}tMBox;


void tMBoxInit(tMBox* box, void **msgBuffer, uint32_t maxCount);
uint32_t tMboxWait(tMBox* box, void** msg, uint32_t waitTicks);
uint32_t tMboxNoWaitGet(tMBox* box, void** msg);
uint32_t tMboxNotify(tMBox* box, void* msg, uint32_t notifyOption);

#endif
