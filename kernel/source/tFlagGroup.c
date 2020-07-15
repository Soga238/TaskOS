#include ".\include\TinyOS.h"

void tFlagGroupInit(tFlagGroup *flagGroup, uint32_t flags)
{
    tEventInit(&flagGroup->event, EVENT_TYPE_FLAG_GROUP);
    flagGroup->flag = flags;
}

// 检查并消耗掉事件标志
static uint32_t tFlagGroupCheckAndConsume(tFlagGroup *group,
        uint8_t type,
        uint32_t *flag)
{

    uint32_t srcFlag = *flag;
    uint32_t isSet = type & TFLAGGROUP_SET;
    uint32_t isAll = type & TFLAGGROUP_ALL;
    uint32_t isConsume = type & TFLAGGROUP_CONSUME;

    // flagGroup->flags & flags：计算出哪些位为1
    // ~flagGroup->flags & flags:计算出哪位为0
    uint32_t calFlag = isSet ? (group->flag & srcFlag) : (~group->flag & srcFlag);

    // 判断是否有位置位或者被复位
    if ((isAll != 0 && calFlag == srcFlag) ||
        (isAll != 0 && calFlag != 0)) {

        // 是否消耗掉标志位（清除标志位）
        if (isConsume) {
            if (isSet) {

                // 清除为1的标志位，变0
                group->flag &= ~srcFlag;
            } else {

                // 清除为0的标志为，变1
                group->flag |= srcFlag;
            }
        }

        *flag = calFlag;
        return NO_ERROR;
    }

    *flag = calFlag;
    return RESOURCE_UNAVALIABLE;
}

uint32_t tFlagGroupWait(tFlagGroup *group,
                        uint32_t waitType,
                        uint32_t requestFlag,
                        uint32_t *resultFlag,
                        uint32_t waitTicks  )
{

    uint32_t status = tTaskEnterCritical();
    uint32_t result;
    uint32_t flags = requestFlag;

    result = tFlagGroupCheckAndConsume(group, waitType, &flags);
    if (result != NO_ERROR) {

        // 如果事件标志不满足条件，则插入到等待队列中
        currentTask->waitFlagsType = waitType;
        currentTask->eventFlags = requestFlag;
        tEventWait(&group->event, currentTask, NULL, EVENT_TYPE_FLAG_GROUP, waitTicks);
        tTaskExitCritical(status);

        // 再执行一次事件调度，以便于切换到其它任务
        tTaskSched();
        *resultFlag = currentTask->eventFlags;
        result = currentTask->waitEventResult;

    } else {
        *resultFlag = flags;
        tTaskExitCritical(status);
    }

    return result;
}

uint32_t tFlagGroupNoWaitGet(tFlagGroup *group,
                             uint32_t waitType,
                             uint32_t requestFlag,
                             uint32_t *resultFlag   )
{
    uint32_t status = tTaskEnterCritical();
    uint32_t result;
    uint32_t flags = requestFlag;

    result = tFlagGroupCheckAndConsume(group, waitType, &flags);
    *resultFlag = flags;
    tTaskExitCritical(status);
    return result;
}

uint32_t tFlagGroupNotify(tFlagGroup *group,
                          uint8_t isSet,
                          uint32_t flag     )
{
    uint32_t status = tTaskEnterCritical();
    uint32_t sched = 0;

    if (isSet) {
        group->flag |= flag;    // 置1
    } else {
        group->flag &= ~flag;   // 置0
    }

    tList *list = &group->event.waitList;
    tNode *node;
    int32_t i = 0;
    for (node = list->headNode.nextNode; i < tListCount(list); node = node->nextNode, ++i) {
        tTask *task = tNodeParent(node, tTask, linkNode);

        uint32_t flags = task->eventFlags;
        uint32_t result = tFlagGroupCheckAndConsume(group, task->waitFlagsType, &flags);
        if (result == NO_ERROR) {
            task->eventFlags = flags;
            tEventWakeUpTask(&group->event, task, NULL, NO_ERROR);
            sched = 1;
        }
    }

    if (sched) {
        tTaskSched();
    }

    tTaskExitCritical(status);
    return 0;
}

void tFlagGroupGetInfo(tFlagGroup* group, tFlagGroupInfo* info)
{
    uint32_t status = tTaskEnterCritical();

    info->flags = group->flag;
    info->taskCount = tEventWaitCount(&group->event);

    tTaskExitCritical(status);
}

uint32_t tFlagGroupDestroy(tFlagGroup* group)
{
    uint32_t status = tTaskEnterCritical();
    uint32_t count = tEventRemoveAll(&group->event, NULL, DELETE);

    if (count > 0) {
        tTaskSched();   // 有任务恢复，尝试调度
    }

    tTaskExitCritical(status);
    return NO_ERROR;
}
