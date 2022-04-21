#ifndef _H_SYSTEM_CALLS
#define _H_SYSTEM_CALLS

#include <stdint.h>

/**
 * @brief  Unsuspends a process, making it ready to run.
 * This method checks consistency of parrams and then calls setReadyState() function.
 * @note   this method disables interrupt mask to prevent involuntarily relinquishing the processor
 * @param  int32_t processId of process to unsuspend
 * @retval uint16_t priority of a given process which have been removed from hibernation.
 */
uint16_t resume(int32_t processId);

/**
 * @brief  Suspends a process, placing it in hibernation.
 * @note   
 * @param  int32_t processId: a process to be suspended
 * @retval uint16_t priority of a given process which have been transferred to suspended state.
 */
uint16_t suspend(int32_t processId);

#endif // !_H_SYSTEM_CALLS
