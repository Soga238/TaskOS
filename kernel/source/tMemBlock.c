#include ".\include\TinyOS.h"


void tMemBlockInit(tMemBlock *memBlock, uint8_t *memStart, uint32_t blockSize, uint32_t blockCnt)
{
    uint8_t *memBlockStart = (uint8_t *)memStart;
    uint8_t *memBlockEnd = memBlockStart + blockSize * blockCnt;


    if (blockSize < sizeof(tNode)) {
        return ;
    }

    tEventInit(&memBlock->event, EVENT_TYPE_MEM_BLOCK);

    memBlock->memStart = memStart;
    memBlock->blockSize = blockSize;
    memBlock->maxCount = blockCnt;

    tListInit(&memBlock->blockList);

    while (memBlockStart < memBlockEnd) {
        tNodeInit((tNode *)memBlockStart);
        tListAddLast(&memBlock->blockList, (tNode *)memBlockStart);
        memBlockStart += blockSize;
    }

}

uint32_t tMemBlockWait(tMemBlock *memBlock, void **mem, uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();

    // 有内存块立即返回
    if (tListCount(&memBlock->blockList) > 0) {
        *mem = tListRemoveFirst(&memBlock->blockList);
        tTaskExitCritical(status);
        return NO_ERROR;

    } else {

        // 没有内存块，插入事件等待链表
        tEventWait(&memBlock->event, currentTask, 0, EVENT_TYPE_MEM_BLOCK, waitTicks);
        tTaskExitCritical(status);

        // 切换任务
        tTaskSched();

        *mem = currentTask->eventMsg;
        return currentTask->waitEventResult;
    }
}

uint32_t tMemBlockNoWaitGet(tMemBlock *memBlock, void **mem)
{
    uint32_t status = tTaskEnterCritical();

    // 有内存块立即返回
    if (tListCount(&memBlock->blockList) > 0) {
        *mem = tListRemoveFirst(&memBlock->blockList);
        tTaskExitCritical(status);
        return NO_ERROR;
    } else {
        tTaskExitCritical(status);
        return RESOURCE_UNAVALIABLE;
    }
}

void tMemBlockNotify(tMemBlock* memBlock, void* mem)
{
    uint32_t status = tTaskEnterCritical();


    if (tEventWaitCount(&memBlock->event) > 0) {
        tTask *task = tEventWakeUp(&memBlock->event, mem, NO_ERROR);
        if (task->prio > currentTask->prio) {
            tTaskSched();
        }
    } else {
        tListAddLast(&memBlock->blockList, (tNode *)mem);
    }

    tTaskExitCritical(status);
}

uint32_t tMemBlockDestroy(tMemBlock* memBlock)
{
    uint32_t status = tTaskEnterCritical();
    uint32_t count = tEventRemoveAll(&memBlock->event, NULL, DELETE);

    tTaskExitCritical(status);

    if (count > 0) {
        tTaskSched();
    }

    return count;
}


void tMemBlockGetInfo(tMemBlock* memBlock, tMemBlockInfo *info)
{
    uint32_t status = tTaskEnterCritical();

    info->count = tListCount(&memBlock->blockList); // 空闲存储块
    info->maxCount = memBlock->maxCount;
    info->taskCount = tEventWaitCount(&memBlock->event);    // 等待内存块的任务链表
    info->blockSize = memBlock->blockSize;

    tTaskExitCritical(status);
}
