#ifndef _H_GET_ITEM
#define _H_GET_ITEM

#include "xinu.h"

/**
 * @brief  Removes a process from the front of a queue.
 * @note   
 * @param  queueId: ID of queue from which to remove a process (assumed valid with no check)
 * @retval QueueEntry.nextNode
 */
int32_t getFirst(int16_t queueId);

/**
 * @brief  Removes a process from end of queue
 * @note   
 * @param  queueId: ID of queue from which to remove a process (assumed valid with no check)
 * @retval QueueEntry.previousNode
 */
int32_t getLast(int16_t queueId);

/**
 * @brief  Removes a process from an arbirary point in a queue.
 * This function takes process ID as an argument and extracts the process from the list
 * in which the process is currently linked. Extraction consists of making the previous node
 * point to the successor and the successor point to the previous node.
 * @note   
 * @param  processId: ID of process to remove
 * @retval ID of removed process
 */
int32_t getItem(int32_t processId);

#endif // !_H_GET_ITEM