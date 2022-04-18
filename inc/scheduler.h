#ifndef _H_SCHEDULER
#define _H_SCHEDULER

#include <stdint.h>

#define DEFER_TRUE 1
#define DEFER_FALSE 0
#define QUANTUM 0xA5

#define DEFER_START 1
#define DEFER_STOP 0

typedef struct DeferralControl
{
    int16_t ndefers;
    int8_t attempt;
} Defer;

Defer defer;

int8_t preemption;
int8_t readyList;   // contains the queue ID of the readyList

/**
 * @brief  Reschedules processor to highest priority eligible process.
 * Assumes interrupts are disabled.
 * @note   Deferred rescheduling is provided for situations where the operating system must make multiple
 * processes eligible to use the processor before allowing any to run.
 * @retval None
 */
void scheduler (void);

/**
 * @brief  
 * @note   
 * @param  defRequest: 
 * @retval 
 */
uint16_t isReschedulingAllowed(uint32_t defRequest);

#endif // !_H_SCHEDULER