#ifndef _H_SYSTEM_CALLS
#define _H_SYSTEM_CALLS

#include <stdint.h>
#include "globals.h"

/**
 * @brief  Unsuspends a process, making it ready to run.
 * This method checks consistency of parrams and then calls scheduler_set_ready_state() function.
 * @note   this method disables interrupt mask to prevent involuntarily relinquishing the processor
 * @param  PID32: processId of process to unsuspend
 * @retval PRIO16: uint16_t priority of a given process which
 *                  have been removed from hibernation. Otherwise:
 *                  SW_BAD_PROCESS_ID or SW_BAD_PROCESS_STATE
 */
PRIO16 syscall_resume(PID32 processId);

/**
 * @brief  Suspends a process, placing it in hibernation.
 * @note   
 * @param  PID32: processId a process to be suspended
 * @retval uint16_t initial (before this method was called) priority value of a given process which have been
 *                  transitioned to suspended state. Otherwise: SW_BAD_PROCESS_ID or SW_BAD_PROCESS_STATE
 */
PRIO16 syscall_suspend(PID32 processId);

/**
 * @brief syscall_kill a process and q_remove it from the system.  
 * Unlike syscall_suspend(), which saves information about a process, syscall_kill() implements process
 * termination by completely removing a process from the system. Released memory in the process
 * table can be reused by a new process.
 * @note   
 * @param  PID32: 
 * @retval STATUS: 
 */
STATUS syscall_kill(PID32 processId);

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
 * @brief  TODO: add description
 * @note   
 * @param  stackSize: 
 * @retval 
 */
uint32_t syscall_round_mb(uint32_t stackSize);

/**
 * @brief  Prints system completion message as last process exits.
 * In some systems, this function powers down the device, in other - restarts. In current implementation
 * this function prints message and halts processor.
 * @note   
 * @retval None
 */
void syscall_xdone(void);

/**
 * @brief  Returns the scheduling priority of a process
 * @note   
 * @param  processId: 
 * @retval PRIO16: Priority value.
 */
PRIO16 syscall_get_process_priority(PID32 processId);

/**
 * @brief  Returns the ID of the currently executing process
 * @note   
 * @retval 
 */
PID32 syscall_get_process_id(void);

/**
 * @brief  Changes the scheduling priority of a process
 * @note   
 * @param  processId: 
 * @param  newPriority: 
 * @retval PRIO16: previous priority value
 */
PRIO16 syscall_change_priority(PID32 processId, PRIO16 newPriority);

#endif // !_H_SYSTEM_CALLS
