#include "../inc/xinu.h"

Node queueTable[QTAB_TOTAL_ENTRIES];

PID32 q_get_first(QID16 queueId)
{
    PID32 head;

    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    head = queueId;
    return q_get_item(queueTable[head].next);
}

PID32 q_get_last(QID16 queueId)
{
    PID32 tail;

    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    tail = (queueId + 1);
    return q_get_item(queueTable[tail].previous);
}

PID32 q_get_item(PID32 processId)
{
    PID32 previous, next;

    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;
    
    next = queueTable[processId].next;
    previous = queueTable[processId].previous;
    
    queueTable[previous].next = next;
    queueTable[next].previous = previous;

    return processId;
}

PID32 q_add(PID32 processId, QID16 queueId)
{
    QID16 tail, previous;

    if (IS_BAD_QUEUE_ID(queueId))
        return SW_BAD_QUEUE_ID;
    
    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;
    
    tail = (queueId + 1);
    previous = queueTable[tail].previous;

    queueTable[processId].next = tail;
    queueTable[processId].previous = previous;

    queueTable[previous].next = processId;
    queueTable[tail].previous = processId;

    return processId;
}

PID32 q_remove(QID16 queueId)
{
    int32_t processId;

    if (IS_BAD_QUEUE_ID(queueId))
        return SW_BAD_QUEUE_ID;
    
    if (IS_EMPTY(queueId))
        return QTAB_EMPTY;
    
    processId = q_get_first(processId);
    queueTable[processId].previous = QTAB_EMPTY;
    queueTable[processId].next = QTAB_EMPTY;

    return processId;
}

SW q_insert(PID32 processId, QID16 queueId, KID32 keyId)
{
    // current - runs through items in a queue
    // previous - holds previous node index
    QID16 current, previous;

    if (IS_BAD_QUEUE_ID(queueId))
        return SW_BAD_QUEUE_ID;
    
    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;
    
    current = GET_FIRST_ID(queueId);

    while (queueTable[current].key >= keyId)
        current = queueTable[current].next;
    
    previous = queueTable[current].previous;
    
    queueTable[processId].next = current;
    queueTable[processId].previous = previous;
    
    queueTable[processId].key = keyId;
    
    queueTable[previous].next = processId;
    queueTable[current].previous = processId;

    return SW_OK;
}

QID16 q_new_queue(void)
{
    static QID16 nextQueueId = ACTIVE_PROCESSES;
    QID16 newQueueId = nextQueueId;

    if (newQueueId > QTAB_TOTAL_ENTRIES)
        return SW_QUEUE_TABLE_IS_FULL;
    
    nextQueueId += 2;

    // Initializing header node
    queueTable[newQueueId].next = (newQueueId + 1);
    queueTable[newQueueId].previous = QTAB_EMPTY;
    queueTable[newQueueId].key = QTAB_MAX_KEY;

    // Initializing tail node
    queueTable[(newQueueId + 1)].next = QTAB_EMPTY;
    queueTable[(newQueueId + 1)].previous = newQueueId;
    queueTable[(newQueueId + 1)].key = QTAB_MIN_KEY;

    return newQueueId;
}