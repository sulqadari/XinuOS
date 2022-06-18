#ifndef _H_PROCESS
#define _H_PROCESS

#include "globals.h"

// inline code to check process ID (assumes)
#define IS_BAD_PROCESS_ID(x)    (  ((PID32)(x) < 0)\
                                || (((PID32)(x) >= ACTIVE_PROCESSES))\
                                || (processTable[(x)].state == PROCESS_STATE_FREE) )

/**
 * @brief  Reschedules processor to highest priority eligible process.
 * 
 * At any time, the highest priority eligible process is executing.
 * Among processes with eual priority scheduling is round-robin.
 * This function is called by running process to give up the processor.
 * 
 * Changing proc_reschedule() and proc_set_ready_state() changes the basic scheduling policy.
 * 
 * @note   Assumes interrupts are disabled.
 * @retval None
 */
void proc_reschedule(void);

/**
 * @brief  Temporarily defers scheduling.
 * 
 * Some OS functions move multiple processes onto the readyList[] at the same time. The important idea
 * is that one or more of the processes that become ready may have a highest priority than the process that is currently executing.
 * However rescheduling in the midst of such a move can result in incomplete and incorrect operation.
 * In particular, rescheduling after making one process ready may cause the process to execute, even if other processes
 * have higher priority.
 * Thus, this function temporarily suspends the scheduling policy.
 * 
 * When deferral is requested the code increments Defer.defersCounter variable.
 * Later, when a function ends its deferral period, Defer.defersCounter is decremented.
 * As long as count remain positive, proc_reschedule() only records that a call was made,
 * but returns to its caller without switching context.
 * When Defer.defersCounter reaches zero, this function examines Defer.attempt to see
 * if proc_reschedule() was called during the deferral period. If so, proc_reschedule() is invoked before returning to its caller.
 * 
 * @note   Assumes interrupts are disabled. Called from proc_switch_context() and proc_set_ready_state()
 * @param  int32_t defReq: DEFER_START - to defer rescheduling; DEFER_STOP - to end a deferral period and continue normal execution.
 * @retval SW: SW_OK, SW_DEFER_HANDLING_EXC or SW_DEFER_UNKNOWN_CMD_EXC
 */
SW proc_defer_handler(int32_t defReq);

/**
 * @brief Stores context of current process into its stack:
 * 
 * 1. push the contents of the processor registers on the stack;
 * 2. save the currStackPtr in Process.stackPointer;
 * 3. load the nextStackPtr of the 'new' process;
 * 4. reload the processor registers from values previously saved on the stack of 'new' process;
 * 5. return to the function in the new process that called this funtion earlier.
 * 
 * @note   
 * @param  uint8_t* currStackPtr: stack pointer of the process which called this function
 * @param  uint8_t* nextStackPtr: stack pointer of new process
 * @retval None
 */
void proc_switch_context(uint8_t* currStackPtr, uint8_t* nextStackPtr);

/**
 * @brief  Makes a process eligible to execute.
 * Scheduling policy specifies that at any time, the highest priority eligible process must be executing.
 * Also each OS function should maintain a scheduling invariant: a function assumes that the highest priority
 * process was executing when the function was called, and must ensure that the highest priority process
 * is executing when the function returns. Thus if a function changes the state of processes, the function must
 * call proc_reschedule() to reestablish the invariant. Thus, when it places a high priority process on the ready
 * list, this proc_set_ready_state() calls proc_reschedule() to ensure that the policy is followed.
 * @note   
 * @param  PID32 processId: a process to be inserted into readyList
 * @retval SW: SW_OK or SW_BAD_PROCESS_ID
 */
SW proc_set_ready_state(PID32 processId);


/**
 * @brief  returns process ID available for allocation of the new process
 * @note   
 * @retval PID32 or SW_FAILED_TO_ALLOCATE_PROCESS_ID
 */
PID32 proc_alloc_process_id(void);


/**
 * @brief  TODO: requires implementation
 * @note   
 * @param  processId: 
 * @retval None
 */
void proc_unsleep(PID32 processId);

/**
 * @brief  Unsuspends a process, making it ready to run.
 * This method checks consistency of parrams and then calls proc_set_ready_state() function.
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
 * @brief kills a process and removes it from the system.  
 * Unlike syscall_suspend(), which saves information about a process, syscall_kill() implements process
 * termination by completely removing a process from the system. Released memory in the process
 * table can be reused by a new process.
 * @note   
 * @param  PID32: 
 * @retval STATUS: 
 */
STATUS syscall_kill(PID32 processId);

/**
 * @brief  Prints system completion message as last process exits.
 * In some systems, this function powers down the device, in other - restarts. In current implementation
 * this function prints message and halts processor.
 * @note   
 * @retval None
 */
void syscall_xdone(void);

#endif // !_H_PROCESS
