#ifndef _H_GET_ITEM
#define _H_GET_ITEM

#include <stdint.h>

/**
 * @brief  Removes a process from the front of a given queue.
 * This function takes a queue ID as an argument, verifies that the argument
 * identifies a nonempty list, finds the process at the head of the list,
 * and cals getItem() function to extract the process from the list.
 * @note   
 * @param  queueId: ID of queue from which to remove a process (assumed valid with no check)
 * @retval QueueEntry.nextNode process that has been successfully extracted
 */
int32_t getFirst(int16_t queueId);

/**
 * @brief  Removes a process from end of a given queue.
 * Similarly to getFirst() function this one takes a queue ID as an argument,
 * validates it, finds the process at the tail of the list and calls getItem()
 * to extract the process.
 * @note   
 * @param  queueId: ID of queue from which to remove a process (assumed valid with no check)
 * @retval QueueEntry.previousNode process that has been successfully extracted
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