#ifndef TMEMBLOCK_H
#define TMEMBLOCK_H


#include ".\tEvent.h"

typedef struct {

    tEvent event;

    void *memStart;

    uint32_t blockSize;

    uint32_t maxCount;  // 内存块最大数量

    tList blockList;

} tMemBlock;

typedef struct {
    uint32_t count;     // 消息数
    uint32_t maxCount;
    uint32_t taskCount;
    uint32_t blockSize;

} tMemBlockInfo;

void tMemBlockInit(tMemBlock* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t blockCnt);
uint32_t tMemBlockWait(tMemBlock* memBlock, void** mem, uint32_t waitTicks);
uint32_t tMemBlockNoWaitGet(tMemBlock* memBlock, void** mem);
void tMemBlockNotify(tMemBlock* memBlock, void* mem);


#endif // !TMEMBLOCK_H
