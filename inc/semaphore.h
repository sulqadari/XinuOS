#ifndef _H_SEMAPHORE
#define _H_SEMAPHORE

#include "globals.h"

#define HAS_BAD_SEMAPHORE_ID(S) ( ((int32_t)(S) < 0) || ((S) >= SEM_TOTAL) )

/**
 * @brief  Causes current process to wait on a semaphore
 * @note   
 * @param  semaphoreId: ID of semaphore on which to wait
 * @retval SW_OK or SW_BAD_SEMAPHORE_ID or SW_BAD_SEMAPHORE_STATE
 */
SW sem_wait(SID32 semaphoreId);

/**
 * @brief  Signals a semaphore, releasing a process if one is waiting
 * @note   
 * @param  semaphoreId: ID of semaphore to signal
 * @retval SW_OK or SW_BAD_SEMAPHORE_ID or SW_BAD_SEMAPHORE_STATE
 */
SW sem_signal(SID32 semaphoreId);

/**
 * @brief  Creates a new semaphore and returns the ID to the caller
 * @note   
 * @param  count: 
 * @retval SID32 or 
 */
SID32 sem_create(int32_t count);

/**
 * @brief  Allocates an unused semaphore and returns its index
 * @note   
 * @retval SID32 or SW_FAILED_TO_ALLOCATE_SID
 */
SID32 sem_get_new_sid(void);

/**
 * @brief  Deletes a semaphore by releasing its able entry.
 * All associated processes will be moved to ready list
 * @note   
 * @param  semaphoreId: 
 * @retval 
 */
SW sem_delete(SID32 semaphoreId);

/**
 * @brief  Resets a semaphore's count and releases waiting processes
 * @note   
 * @param  semaphoreId: 
 * @param  count: 
 * @retval 
 */
SW sem_reset(SID32 semaphoreId, int32_t count);

#endif // !_H_SEMAPHORE
