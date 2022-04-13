#include "../inc/xinu.h"

int16_t newQueue(void)
{
    static int16_t nextQueueId = PROCESS_NUMBER;
    int16_t newQueueId = nextQueueId;

    if (newQueueId > QTAB_TOTAL_OF_PROCESSES)
        return STATUS_ERROR;
    
    nextQueueId += 2;

    queueTable[GET_QUEUE_HEAD(newQueueId)].nextNode = GET_QUEUE_TAIL(newQueueId);
    queueTable[GET_QUEUE_HEAD(newQueueId)].previousNode = QTAB_EMPTY;
    queueTable[GET_QUEUE_HEAD(newQueueId)].key = QTAB_MAX_KEY;

    queueTable[GET_QUEUE_TAIL(newQueueId)].nextNode = QTAB_EMPTY;
    queueTable[GET_QUEUE_TAIL(newQueueId)].previousNode = GET_QUEUE_HEAD(newQueueId);
    queueTable[GET_QUEUE_TAIL(newQueueId)].key = QTAB_MIN_KEY;

    return newQueueId;
}