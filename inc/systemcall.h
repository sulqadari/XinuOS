#ifndef _H_SYSTEM_CALLS
#define _H_SYSTEM_CALLS

#include <stdint.h>

/**
 * @brief  Unsuspends a process, making it ready to run.
 * This method checks consistency of parrams and then calls set_ready_state() function.
 * @note   this method disables interrupt mask to prevent involuntarily relinquishing the processor
 * @param  PID32: processId of process to unsuspend
 * @retval PRIO16: uint16_t priority of a given process which
 *                  have been removed from hibernation. Otherwise:
 *                  SW_BAD_PROCESS_ID or SW_BAD_PROCESS_STATE
 */
PRIO16 resume(PID32 processId);

/**
 * @brief  Suspends a process, placing it in hibernation.
 * @note   
 * @param  PID32: processId a process to be suspended
 * @retval uint16_t priority of a given process which have been
 *                  transferred to suspended state. Otherwise:
 *                  SW_BAD_PROCESS_ID or SW_BAD_PROCESS_STATE
 */
PRIO16 suspend(PID32 processId);

/**
 * @brief Kill a process and remove it from the system.  
 * Unlike suspend(), which saves information about a process, kill() implements process
 * termination by completely removing a process from the system. Released memory in the process
 * table can be reused by a new process.
 * @note   
 * @param  PID32: 
 * @retval STATUS: 
 */
STATUS kill(PID32 processId);

/**
 * @brief  Creates a new process.
 * In essence, this method builds an image of the process as if it had been stopped while running.
 * Once the image has been constructed and the process has been placed on the ready list, 
 * hal_switch_context() can switch to it.
 * This function forms a saved environment on the process's stack as if the specified function
 * had been called. Consequently, we refer to the initial configuration as a pseudo call.
 * To build a pseudo call, create() stores initial values for the registers, including the stack
 * pointer and a return address in the pseudo-call on the process's stack. When hal_switch_context()
 * switches to it, the new process begins executing the code for the designated function, obeying the normal
 * calling conventions for accessing arguments and allocating local variables.
 * In short, the intial function for a process behaves exactly as if it had been called.
 * But what value should create() use as a return address in the pseudo-call? The value
 * determines what action the system will take if a process returns from its initial
 * (i.e. top-level) function. The paradigm is well-known:
 * If a process returns from the initial (top-level) function in which its execution started,
 * the process exits.
 * To arrange for an exit to occur when the initial call returns, create() assigns the address
 * of function returnAddress as the return address in the pseudo call.
 * Create also fills in the process table entry.
 * @note   
 * @param  funcAddress: specifies the initial function at which the process should start execution.
 * @param  stackSize: 
 * @param  priority: 
 * @param  name: 
 * @param  nargs: 
 * @retval 
 */
PID32 create(void* funcAddress, uint32_t stackSize, PRIO16 priority, uint8_t* name, uint32_t nargs);

/**
 * @brief  returns process ID available for allocation of the new process
 * @note   
 * @retval PID32 or SW_FAILED_TO_ALLOCATE_PROCESS_ID
 */
PID32 get_new_process_id(void);

/**
 * @brief  
 * @note   
 * @retval None
 */
void returnAddress(void);

/**
 * @brief  Prints system completion message as last process exits.
 * In some systems, this function powers down the device, in other - restarts. In current implementation
 * this function prints message and halts processor.
 * @note   
 * @retval None
 */
void xdone(void);
#endif // !_H_SYSTEM_CALLS
