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
 * @note   Assumes interrupts are disabled.
 * @retval None
 */
void doRescheduling(void);

/**
 * @brief  Temporarily defers scheduling.
 * Some OS functions move multiple processes onto the readyList[] at the same time. The important idea
 * is that one or more of the processes that become ready may have a highest priority than the process that is currently executing.
 * However rescheduling in the midst of such a move can result in incomplete and incorrect operation.
 * In particular, rescheduling after making one process ready may cause the process to execute, even if other processes
 * have higher priority.
 * Thus, this function temporarily suspends the scheduling policy.
 * 
 * 
 * When deferral is requested the code increments Defer.ndefers variable.
 * Later, when a function ends its deferral period, Defer.ndefers is decremented.
 * As long as count remain positive, doRescheduling() only records that a call was made,
 * but returns to its caller without switching context.
 * When Defer.ndefers reaches zero, this function examines Defer.attempt to see
 * if doRescheduling() was called during the deferral period. If so, doRescheduling() is invoked before returning to its caller.
 * 
 * 
 * @note   
 * @param  uint32_t defReq: DEFER_START - to defer rescheduling; DEFER_STOP - to end a deferral period and continue normal execution.
 * @retval STATUS_OK, STATUS_DEFER_HANDLING_EXC or STATUS_DEFER_UNKNOWN_CMD_EXC
 */
uint16_t isReschedulingAllowed(uint32_t defReq);

#endif // !_H_SCHEDULER