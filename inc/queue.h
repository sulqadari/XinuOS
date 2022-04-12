/**
 * @file queue.h
 * @author TIslamov (you@domain.com)
 * @brief Queue structure declarations, constants and inline functions.
 * @version 0.1
 * @date 2022-04-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _H_QUEUE_TABLE
#define _H_QUEUE_TABLE

#include "xinu.h"

/* Default number of queue entries:
 * 1 per process +
 * 2 for ready list + 2 for sleep list +
 * semaphore + semaphore
 */
#define QTAB_TOTAL_OF_ENTRIES   (PROCESS_QUANTITY + 4 + NSEM + NSEM)        //NQENT (NPROC + 4 + NSEM + NSEM)
#define QTAB_EMPTY      (-1)
#define QTAB_MAX_KEY    0x7FFFFFFF
#define QTAB_MIN_KET    0x80000000

typedef struct QueueEntry {
    int32_t key;
    uint16_t nextNode;
    uint16_t previousNode;
} Entry;

/*
* Contains QTAB_TOTAL_OF_ENTRIES entries.
* An important implicit boundary occurs between element PROCESS_QUANTITY - 1 and PROCESS_QUANTITY.
* Each element below the boundary corresponds to a process ID,
* and the elements queueTable[PROCESS_QUANTITY] through queueTable[QTAB_TOTAL_OF_ENTRIES]
* correspond to the heads or tails of lists.
*/
extern Entry queueTable[];

// Queue manipulation functions
#define QTAB_QUEUE_HEAD(q)  (q)
#define QTAB_QUEUE_TAIL(q)  ((q) + 1)

#define QTAB_FIRST_ID(q)    (queueTable[QTAB_QUEUE_HEAD(q)].nextNode)
#define QTAB_LAST_ID(q)     (queueTable[QTAB_QUEUE_TAIL(q)].previousNode)

// Both inline functions check if the given node on a list is a process or the list head/tail.
// The node is process (not head, nor tail) if its index is less than PROCESS_QUANTITY
#define QTAB_IS_EMPTY(q)    (QTAB_FIRST_ID(q) >= PROCESS_QUANTITY)
#define QTAB_NON_EMPTY(q)   (QTAB_FIRST_ID(q) <  PROCESS_QUANTITY)

#define QTAB_FIRST_KEY(q)   (queueTable[QTAB_FIRST_ID(q)].key)
#define QTAB_LAST_KEY(q)    (queueTable[QTAB_LAST_ID(q) ].key)

// Inline to check queue ID assumes interrupts are disabled
#define QTAB_IS_BAD_QUEUE_ID(id)    (((int32_t)(id) < 0)|| (int32_t)(id) >= (QTAB_TOTAL_OF_ENTRIES - 1))

#endif // !_H_QUEUE_TABLE