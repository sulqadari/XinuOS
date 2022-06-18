#ifndef _H_SYSTEM_CALLS
#define _H_SYSTEM_CALLS

#include <stdint.h>
#include "globals.h"

/**
 * @brief  Creates a new process.
 * In essence, this method builds an image of the process as if it had been stopped while running.
 * To arrange for an exit to occur when the initial call returns, this function assigns the address
 * of function "syscall_return_address" as the return address in the pseudo call.
 * syscall_create_process also fills in the process table entry.
 * 
 * @note   
 * @param  funcAddress: specifies the initial function at which the process should start execution.
 * @param  stackSize: 
 * @param  priority: 
 * @param  name: 
 * @param  nargs: 
 * @retval PID32: the ID of the newly created process.
 */
PID32 syscall_create_process(void* funcAddress, uint32_t stackSize, PRIO16 priority, uint8_t* name, uint32_t nargs);

/**
 * @brief  Defines an address to which process should return when it terminats.
 * In fact this address is an entry point to this function which implements logic
 * intended to terminate process.
 * @note   
 * @retval None
 */
void syscall_return_address(void);

/**
 * @brief  Returns the ID of the currently executing process
 * @note   
 * @retval 
 */
PID32 syscall_get_process_id(void);

/**
 * @brief  Returns the scheduling priority of a process
 * @note   
 * @param  processId: 
 * @retval PRIO16: Priority value.
 */
PRIO16 syscall_get_process_priority(PID32 processId);

/**
 * @brief  Changes the scheduling priority of a process
 * @note   
 * @param  processId: 
 * @param  newPriority: 
 * @retval PRIO16: previous priority value
 */
PRIO16 syscall_set_process_priority(PID32 processId, PRIO16 newPriority);

/**
 * @brief  Passes a message to a process and starts recipient if waiting
 * @note   see section 8.5
 * @param  processId: ID of recipient process
 * @param  msg: contents of message
 * @retval SW_OK
 */
SW syscall_send_msg(PID32 processId, uMSG32 msg);

/**
 * @brief  Waits for a message and returns it to the caller
 * @note   
 * @retval uMSG32
 */
uMSG32 syscall_receive_msg(void);

/**
 * @brief  Non-blocking version of syscall_receive_msg().
 * It clears incoming message and returns message if one waiting. A process can use this
 * function to remove an old message before starting an interaction that uses message passing.
 * @note   
 * @retval 
 */
uMSG32 syscall_nb_receive_msg(void);

#endif // !_H_SYSTEM_CALLS
