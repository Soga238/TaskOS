#include ".\include\TinyOS.h"

void tTaskDelay(uint32_t wTicks)
{
    uint32_t status = tTaskEnterCritical();

    tTimeTaskWait(currentTask, wTicks); // 将当前任务插入延时链表
    tTaskScedUnRdy(currentTask); // 从就绪任务表中删除当前任务

    tTaskSched();
    
    tTaskExitCritical(status);
}
