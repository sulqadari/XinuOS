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

#include <stdint.h>
#include "globals.h"
#include "semaphore.h"

/* Default number of queue entries:
 * 1 per process +
 * 2 ready list +
 * 2 sleep list +
 * 2 semaphore
 */
#define QTAB_TOTAL_OF_PROCESSES   (MAX_NUM_OF_ACTIVE_PROCESSES + 4 + NUMBER_OF_SEMAPHORES + NUMBER_OF_SEMAPHORES)        //NQENT (NPROC + 4 + NUMBER_OF_SEMAPHORES + NUMBER_OF_SEMAPHORES)
#define QTAB_EMPTY      (-1)
#define QTAB_MAX_KEY    0x7FFFFFFF
#define QTAB_MIN_KEY    0x80000000

typedef struct QueueTableEntry
{
    KID32 key;
    QID16 next;
    QID16 previous;
} Node;

/*
 * Contains QTAB_TOTAL_OF_PROCESSES entries. Represents a doubly linked list data structure.
 * An important implicit boundary occurs between element MAX_NUM_OF_ACTIVE_PROCESSES - 1 and MAX_NUM_OF_ACTIVE_PROCESSES.
 * Each element below the boundary corresponds to a process ID,
 * and the elements queueTable[MAX_NUM_OF_ACTIVE_PROCESSES] through queueTable[QTAB_TOTAL_OF_PROCESSES]
 * correspond to the heads or tails of lists.
 */
extern Node queueTable[];

// Queue manipulation functions
#define GET_QUEUE_HEAD(processId)  (processId)
#define GET_QUEUE_TAIL(processId)  ((processId) + 1)

#define GET_FIRST_ID(processId)    (queueTable[GET_QUEUE_HEAD(processId)].next)
#define GET_LAST_ID(processId)     (queueTable[GET_QUEUE_TAIL(processId)].previous)

// Both inline functions check if the given node on a list is a process or the list head/tail.
// The node is process (not head, nor tail) if its index is less than MAX_NUM_OF_ACTIVE_PROCESSES
#define IS_EMPTY(processId)    (GET_FIRST_ID(processId) >= MAX_NUM_OF_ACTIVE_PROCESSES)
#define NOT_EMPTY(processId)   (GET_FIRST_ID(processId) <  MAX_NUM_OF_ACTIVE_PROCESSES)

#define GET_FIRST_KEY(keyId)   (queueTable[GET_FIRST_ID(keyId)].key)
#define GET_LAST_KEY(keyId)    (queueTable[GET_LAST_ID(keyId) ].key)

// Inline to check queue ID. Assumes interrupts are disabled.
#define IS_BAD_QUEUE_ID(queueId)    ( ((int32_t)(queueId) < 0) || \
                                    (int32_t)(queueId) >= (QTAB_TOTAL_OF_PROCESSES - 1) )

/**
 * @brief  Removes a process from the front of a given queue.
 * This function takes a queue ID as an argument, verifies that the argument
 * identifies a nonempty list, finds the process at the head of the list,
 * and cals q_get_item() function to extract the process from the list.
 * @note   
 * @param  QID16 queueId: ID of queue from which to q_remove a process (assumed valid with no check)
 * @retval PID32 QueueTableEntry.next process that has been successfully extracted
 */
PID32 q_get_first(QID16 queueId);

/**
 * @brief  Removes a process from end of a given queue.
 * Similarly to q_get_first() function this one takes a queue ID as an argument,
 * validates it, finds the process at the tail of the list and calls q_get_item()
 * to extract the process.
 * @note   
 * @param  QID16 queueId: ID of queue from which to q_remove a process (assumed valid with no check)
 * @retval PID32 QueueTableEntry.previous process that has been successfully extracted
 */
PID32 q_get_last(QID16 queueId);

/**
 * @brief  Removes a process from an arbirary point in a queue.
 * This function takes process ID as an argument and extracts the process from the list
 * in which the process is currently linked. Extraction consists of making the previous node
 * point to the successor and the successor point to the previous node.
 * @note   
 * @param  PID32 processId: ID of process to q_remove
 * @retval PID32 ID of removed process
 */
PID32 q_get_item(PID32 processId);

/**
 * @brief  Inserts a process at the tail of a queue
 * @note   
 * @param  PID32 processId: ID of process to q_insert
 * @param  QID16 queueId: ID of queue to use
 * @retval PID32 process ID that has been inserted prior to the tail of a list.
 */
PID32 q_add(PID32 processId, QID16 queueId);

/**
 * @brief  Removes and returns the first process (after head node) on a list
 * @note   
 * @param  QID16 queueId: ID queue to use
 * @retval PID32 process ID removed from the list. Otherwise QTAB_EMPTY if list is empty 
 */
PID32 q_remove(QID16 queueId);

/**
 * @brief  Inserts a process into a queue in descending key order.
 * To find the correct location in the list, this method searches for an existing
 * element with a key less than the key of the element being inserted. 
 * @note   
 * @param  int32_t processId: process to be inserted
 * @param  int16_t queueId: a queue on which to q_insert the process
 * @param  int32_t keyId: an integer priority for the process
 * @retval STATUS SW_OK in case of success.
 */
STATUS q_insert(PID32 processId, QID16 queueId, KID32 keyId);

/**
 * @brief  allocates and initializes a queue in the global queue table
 * @note   
 * @retval QID16 ID of allocated queue
 */
QID16 q_new_queue(void);

#endif // !_H_QUEUE_TABLE
