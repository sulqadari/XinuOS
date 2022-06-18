#ifndef _H_QUEUE_TABLE
#define _H_QUEUE_TABLE

#include "globals.h"
#include "semaphore.h"

#define GET_FIRST_ID(queueId)   (queueTable[queueId].next)
#define GET_LAST_ID(queueId)  (queueTable[(queueId + 1)].previous)

// Both inline functions check if the given node on a list is a process or the list head/tail.
// The node is process (not head, nor tail) if its index is less than ACTIVE_PROCESSES
// see section 4.4
#define IS_EMPTY(queueId)    (GET_FIRST_ID(queueId) >= ACTIVE_PROCESSES)
#define NOT_EMPTY(queueId)   (GET_FIRST_ID(queueId) <  ACTIVE_PROCESSES)

#define GET_FIRST_KEY(keyId)   (queueTable[GET_FIRST_ID(keyId)].key)
#define GET_LAST_KEY(keyId)    (queueTable[GET_LAST_ID(keyId) ].key)

// Inline to check queue ID. Assumes interrupts are disabled.
#define IS_BAD_QUEUE_ID(queueId)    ( (((QID16)queueId) < ((QID16)0)) || (((QID16)queueId) >= (QTAB_TOTAL_ENTRIES - (QID16)1)) )

/**
 * @brief  Removes a process from the front of a given queue.
 * This function takes a queue ID as an argument, verifies that the argument
 * identifies a non-empty list, finds the process at the head of this list
 * and cals q_get_item() function to extract the process from the list.
 * @note   
 * @param  QID16 queueId: ID of queue from which to q_remove a process (assumed valid with no check)
 * @retval PID32 ListEntry.next process that has been successfully extracted
 */
PID32 q_get_first(QID16 queueId);

/**
 * @brief  Removes a process from end of a given queue.
 * Similarly to q_get_first() function this one takes a queue ID as an argument,
 * validates it, finds the process at the tail of the list and calls q_get_item()
 * to extract the process.
 * @note   
 * @param  QID16 queueId: ID of queue from which to remove a process (assumed valid with no check)
 * @retval PID32 ListEntry.previous process that has been successfully extracted
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
 * @retval SW SW_OK in case of success.
 */
SW q_insert(PID32 processId, QID16 queueId, KID32 keyId);

/**
 * @brief  allocates and initializes a queue in the global queue table
 * @note   
 * @retval QID16 ID of allocated queue
 */
QID16 q_new_queue(void);

#endif // !_H_QUEUE_TABLE
