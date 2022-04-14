#include "../inc/xinu.h"

int32_t add(int32_t processId, int16_t queueId)
{
    int16_t tail, previous;

    if (IS_BAD_QUEUE_ID(queueId))
        return STATUS_BAD_QUEUE_ID;
    
    if (IS_BAD_PROCESS_ID(processId))
        return STATUS_BAD_PROCESS_ID;
    
    tail = GET_QUEUE_TAIL(queueId);
    previous = queueTable[tail].previousNode;

    queueTable[processId].nextNode = tail;
    queueTable[processId].previousNode = previous;

    queueTable[previous].nextNode = processId;
    queueTable[tail].previousNode = processId;

    return processId;
}

int32_t remove(int16_t queueId)
{
    int32_t processId;

    if (IS_BAD_QUEUE_ID(queueId))
        return STATUS_BAD_QUEUE_ID;
    
    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    processId = getFirst(processId);
    queueTable[processId].previousNode = QTAB_EMPTY;
    queueTable[processId].nextNode = QTAB_EMPTY;

    return processId;
}

int32_t getFirst(int16_t queueId)
{
    int32_t head;

    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    head = GET_QUEUE_HEAD(queueId);
    return getItem(queueTable[head].nextNode);
}

int32_t getLast(int16_t queueId)
{
    int32_t tail;

    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    tail = GET_QUEUE_TAIL(queueId);
    return getItem(queueTable[tail].previousNode);
}

int32_t getItem(int32_t processId)
{
    int32_t previous, next;

    if (IS_BAD_PROCESS_ID(processId))
        return STATUS_BAD_PROCESS_ID;
    
    next = queueTable[processId].nextNode;
    previous = queueTable[processId].previousNode;
    
    queueTable[previous].nextNode = next;
    queueTable[next].previousNode = previous;

    return processId;
}

uint16_t insert(int32_t processId, int16_t queueId, int32_t keyId)
{
    // current - runs through items in a queue
    // previous - holds previous node index
    int16_t current, previous;

    if (IS_BAD_QUEUE_ID(queueId))
        return STATUS_BAD_QUEUE_ID;
    
    if (IS_BAD_PROCESS_ID(processId))
        return STATUS_BAD_PROCESS_ID;
    
    current = GET_FIRST_ID(queueId);

    while (queueTable[current].nodeKey >= keyId)
        current = queueTable[current].nextNode;
    
    previous = queueTable[current].previousNode;
    
    queueTable[processId].nextNode = current;
    queueTable[processId].previousNode = previous;
    
    queueTable[processId].nodeKey = keyId;
    
    queueTable[previous].nextNode = processId;
    queueTable[current].previousNode = processId;

    return STATUS_OK;
}

int16_t newQueue(void)
{
    static int16_t nextQueueId = MAX_NUM_OF_ACTIVE_PROCESSES;
    int16_t newQueueId = nextQueueId;

    if (newQueueId > QTAB_TOTAL_OF_PROCESSES)
        return STATUS_QUEUE_TABLE_IS_FULL;
    
    nextQueueId += 2;

    // Initializing header node
    queueTable[GET_QUEUE_HEAD(newQueueId)].nextNode = GET_QUEUE_TAIL(newQueueId);
    queueTable[GET_QUEUE_HEAD(newQueueId)].previousNode = QTAB_EMPTY;
    queueTable[GET_QUEUE_HEAD(newQueueId)].nodeKey = QTAB_MAX_KEY;

    // Initializing tail node
    queueTable[GET_QUEUE_TAIL(newQueueId)].nextNode = QTAB_EMPTY;
    queueTable[GET_QUEUE_TAIL(newQueueId)].previousNode = GET_QUEUE_HEAD(newQueueId);
    queueTable[GET_QUEUE_TAIL(newQueueId)].nodeKey = QTAB_MIN_KEY;

    return newQueueId;
}