#include "../inc/xinu.h"

int32_t getFirst(int16_t queueId)
{
    int32_t head;
    if (QTAB_IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    head = QTAB_QUEUE_HEAD(queueId);
    return getItem(queueTable[head].nextNode);
}

int32_t getLast(int16_t queueId)
{
    int32_t tail;
    if (QTAB_IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    tail = QTAB_QUEUE_TAIL(queueId);
    return getItem(queueTable[tail].previousNode);
}

int32_t getItem(int32_t processId)
{
    int32_t previous, next;
    next = queueTable[processId].nextNode;
    previous = queueTable[processId].previousNode;
    
    queueTable[previous].nextNode = next;
    queueTable[next].previousNode = previous;

    return processId;
}