#include "../inc/xinu.h"

PID32 get_first(QID16 queueId)
{
    PID32 head;

    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    head = GET_QUEUE_HEAD(queueId);
    return get_item(queueTable[head].nextNode);
}

PID32 get_last(QID16 queueId)
{
    PID32 tail;

    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    tail = GET_QUEUE_TAIL(queueId);
    return get_item(queueTable[tail].previousNode);
}

PID32 get_item(PID32 processId)
{
    PID32 previous, next;

    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;
    
    next = queueTable[processId].nextNode;
    previous = queueTable[processId].previousNode;
    
    queueTable[previous].nextNode = next;
    queueTable[next].previousNode = previous;

    return processId;
}

PID32 add(PID32 processId, QID16 queueId)
{
    QID16 tail, previous;

    if (IS_BAD_QUEUE_ID(queueId))
        return SW_BAD_QUEUE_ID;
    
    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;
    
    tail = GET_QUEUE_TAIL(queueId);
    previous = queueTable[tail].previousNode;

    queueTable[processId].nextNode = tail;
    queueTable[processId].previousNode = previous;

    queueTable[previous].nextNode = processId;
    queueTable[tail].previousNode = processId;

    return processId;
}

PID32 remove(QID16 queueId)
{
    int32_t processId;

    if (IS_BAD_QUEUE_ID(queueId))
        return SW_BAD_QUEUE_ID;
    
    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    processId = get_first(processId);
    queueTable[processId].previousNode = QTAB_EMPTY;
    queueTable[processId].nextNode = QTAB_EMPTY;

    return processId;
}

STATUS insert(PID32 processId, QID16 queueId, KID32 keyId)
{
    // current - runs through items in a queue
    // previous - holds previous node index
    QID16 current, previous;

    if (IS_BAD_QUEUE_ID(queueId))
        return SW_BAD_QUEUE_ID;
    
    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;
    
    current = GET_FIRST_ID(queueId);

    while (queueTable[current].nodeKey >= keyId)
        current = queueTable[current].nextNode;
    
    previous = queueTable[current].previousNode;
    
    queueTable[processId].nextNode = current;
    queueTable[processId].previousNode = previous;
    
    queueTable[processId].nodeKey = keyId;
    
    queueTable[previous].nextNode = processId;
    queueTable[current].previousNode = processId;

    return SW_OK;
}

QID16 new_queue(void)
{
    static QID16 nextQueueId = MAX_NUM_OF_ACTIVE_PROCESSES;
    QID16 newQueueId = nextQueueId;

    if (newQueueId > QTAB_TOTAL_OF_PROCESSES)
        return SW_QUEUE_TABLE_IS_FULL;
    
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