#include "../inc/xinu.h"

int32_t queueAdd(int32_t processId, int16_t queueId)
{
    int16_t tail, previous;

    if (IS_BAD_QUEUE_ID(queueId) || IS_BAD_PROCESS_ID(processId))
        return STATUS_ERROR;
    
    tail = GET_QUEUE_TAIL(queueId);
    previous = queueTable[tail].previousNode;

    queueTable[processId].nextNode = tail;
    queueTable[processId].previousNode = previous;

    queueTable[previous].nextNode = processId;
    queueTable[tail].previousNode = processId;

    return processId;
}

int32_t queueRemove(int16_t queueId)
{
    int32_t processId;

    if (IS_BAD_QUEUE_ID(queueId))
        return STATUS_ERROR;
    
    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    processId = getFirst(processId);
    queueTable[processId].previousNode = QTAB_EMPTY;
    queueTable[processId].nextNode = QTAB_EMPTY;

    return processId;
}