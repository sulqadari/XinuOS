#include "../inc/xinu.h"

uint16_t priorityQueueInsert(int32_t processId, int16_t queueId, int32_t keyId)
{
    // current - runs through items in a queue
    // previous - holds previous node index
    int16_t current, previous;

    if (IS_BAD_QUEUE_ID(queueId) || IS_BAD_PROCESS_ID(processId))
        return STATUS_ERROR;
    
    current = GET_FIRST_ID(queueId);
    while (queueTable[current].key >= keyId)
        current = queueTable[current].nextNode;
    
    previous = queueTable[current].previousNode;
    queueTable[processId].nextNode = current;
    queueTable[processId].previousNode = previous;
    queueTable[processId].key = keyId;
    queueTable[previous].nextNode = processId;
    queueTable[current].previousNode = processId;

    return STATUS_OK;
}