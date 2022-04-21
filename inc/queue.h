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

/* Default number of queue entries:
 * 1 per process +
 * 2 ready list +
 * 2 sleep list +
 * 2 semaphore
 */
#define QTAB_TOTAL_OF_PROCESSES   (MAX_NUM_OF_ACTIVE_PROCESSES + 4 + NSEM + NSEM)        //NQENT (NPROC + 4 + NSEM + NSEM)
#define QTAB_EMPTY      (-1)
#define QTAB_MAX_KEY    0x7FFFFFFF
#define QTAB_MIN_KEY    0x80000000

typedef struct QueueTableEntry
{
    int32_t nodeKey;
    int16_t nextNode;
    int16_t previousNode;
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

#define GET_FIRST_ID(processId)    (queueTable[GET_QUEUE_HEAD(processId)].nextNode)
#define GET_LAST_ID(processId)     (queueTable[GET_QUEUE_TAIL(processId)].previousNode)

// Both inline functions check if the given node on a list is a process or the list head/tail.
// The node is process (not head, nor tail) if its index is less than MAX_NUM_OF_ACTIVE_PROCESSES
#define IS_EMPTY(processId)    (GET_FIRST_ID(processId) >= MAX_NUM_OF_ACTIVE_PROCESSES)
#define NOT_EMPTY(processId)   (GET_FIRST_ID(processId) <  MAX_NUM_OF_ACTIVE_PROCESSES)

#define GET_FIRST_KEY(keyId)   (queueTable[GET_FIRST_ID(keyId)].nodeKey)
#define GET_LAST_KEY(keyId)    (queueTable[GET_LAST_ID(keyId) ].nodeKey)

// Inline to check queue ID. Assumes interrupts are disabled.
#define IS_BAD_QUEUE_ID(queueId)    ( ((int32_t)(queueId) < 0) || (int32_t)(queueId) >= (QTAB_TOTAL_OF_PROCESSES - 1) )

/**
 * @brief  Inserts a process at the tail of a queue
 * @note   
 * @param  int32_t processId: ID of process to insert
 * @param  int16_t queueId: ID of queue to use
 * @retval int32_t process ID that has been inserted prior to the tail of a list.
 */
int32_t add(int32_t processId, int16_t queueId);

/**
 * @brief  Removes and returns the first process (after head node) on a list
 * @note   
 * @param  int16_t queueId: ID queue to use
 * @retval int32_t process ID removed from the list. Otherwise QTAB_EMPTY if list is empty 
 */
int32_t remove(int16_t queueId);

/**
 * @brief  Removes a process from the front of a given queue.
 * This function takes a queue ID as an argument, verifies that the argument
 * identifies a nonempty list, finds the process at the head of the list,
 * and cals getItem() function to extract the process from the list.
 * @note   
 * @param  int16_t queueId: ID of queue from which to remove a process (assumed valid with no check)
 * @retval int32_t QueueTableEntry.nextNode process that has been successfully extracted
 */
int32_t getFirst(int16_t queueId);

/**
 * @brief  Removes a process from end of a given queue.
 * Similarly to getFirst() function this one takes a queue ID as an argument,
 * validates it, finds the process at the tail of the list and calls getItem()
 * to extract the process.
 * @note   
 * @param  int16_t queueId: ID of queue from which to remove a process (assumed valid with no check)
 * @retval int32_t QueueTableEntry.previousNode process that has been successfully extracted
 */
int32_t getLast(int16_t queueId);

/**
 * @brief  Removes a process from an arbirary point in a queue.
 * This function takes process ID as an argument and extracts the process from the list
 * in which the process is currently linked. Extraction consists of making the previous node
 * point to the successor and the successor point to the previous node.
 * @note   
 * @param  int32_t processId: ID of process to remove
 * @retval int32_t ID of removed process
 */
int32_t getItem(int32_t processId);

/**
 * @brief  Inserts a process into a queue in descending nodeKey order.
 * To find the correct location in the list, this method searches for an existing
 * element with a nodeKey less than the nodeKey of the element being inserted. 
 * @note   
 * @param  int32_t processId: process to be inserted
 * @param  int16_t queueId: a queue on which to insert the process
 * @param  int32_t keyId: an integer priority for the process
 * @retval int16_t status SW_OK in case of success.
 */
int16_t insert(int32_t processId, int16_t queueId, int32_t keyId);

/**
 * @brief  allocates and initializes a queue in the global queue table
 * @note   
 * @retval int16_t ID of allocated queue
 */
int16_t newQueue(void);

#endif // !_H_QUEUE_TABLE
