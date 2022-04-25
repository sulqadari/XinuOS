#ifndef _H_SCHEDULER
#define _H_SCHEDULER

#include <stdint.h>

#define DEFER_TRUE 1
#define DEFER_FALSE 0
#define QUANTUM 0xA5

#define DEFER_START 1
#define DEFER_STOP 0

/**
 * @brief  Reschedules processor to highest priority eligible process.
 * 
 * At any time, the highest priority eligible process is executing.
 * Among processes with eual priority scheduling is round-robin.
 * This function is called by running process to give up the processor.
 * 
 * Changing reschedule() and set_ready_state() changes the basic scheduling policy.
 * 
 * @note   Assumes interrupts are disabled.
 * @retval None
 */
void reschedule(void);

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
 * As long as count remain positive, reschedule() only records that a call was made,
 * but returns to its caller without switching context.
 * When Defer.defersCounter reaches zero, this function examines Defer.attempt to see
 * if reschedule() was called during the deferral period. If so, reschedule() is invoked before returning to its caller.
 * 
 * @note   Assumes interrupts are disabled. Called from hal_switch_context() and set_ready_state()
 * @param  int32_t defReq: DEFER_START - to defer rescheduling; DEFER_STOP - to end a deferral period and continue normal execution.
 * @retval STATUS: SW_OK, SW_DEFER_HANDLING_EXC or SW_DEFER_UNKNOWN_CMD_EXC
 */
STATUS reschedule_control(int32_t defReq);

/**
 * @brief  Makes a process eligible to execute.
 * Scheduling policy specifies that at any time, the highest priority eligible process must be executing.
 * Also each OS function should maintain a scheduling invariant: a function assumes that the highest priority
 * process was executing when the function was called, and must ensure that the highest priority process
 * is executing when the function returns. Thus if a function changes the state of processes, the function must
 * call reschedule() to reestablish the invariant. Thus, when it places a high priority process on the ready
 * list, this set_ready_state() calls reschedule() to ensure that the policy is followed.
 * @note   
 * @param  PID32 processId: a process to be inserted into readyList
 * @retval STATUS: SW_OK or SW_BAD_PROCESS_ID
 */
STATUS set_ready_state(PID32 processId);

#endif // !_H_SCHEDULER