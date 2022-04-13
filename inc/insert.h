#ifndef _H_INSERT
#define _H_INSERT

#include <stdint.h>

/**
 * @brief  Inserts a process into a queue in descending key order.
 * To find the correct location in the list, this method searches for an existing
 * element with a key less than the key of the element being inserted. 
 * @note   
 * @param  processId: process to be inserted
 * @param  queueId: a queue on which to insert the process
 * @param  keyId: an integer priority for the process
 * @retval status STATUS_OK in case of success.
 */
uint16_t priorityQueueInsert(int32_t processId, int16_t queueId, int32_t keyId);

#endif // !_H_INSERT
