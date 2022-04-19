#ifndef _H_GLOBALS
#define _H_GLOBALS

/**
 * @brief  Global counter of requests to defer rescheduling.
 * @note   
 * @retval None
 */
typedef struct DeferralControl
{
    int16_t ndefers;
    int8_t attempt;
} Defer;

// Global counter of requests to defer rescheduling.
Defer defer;

// stores ID of currently running process.
// Current process does not appear on the ready list. To provide fast 
// access to the current process, its ID is stored in this variable.
extern uint32_t currentProcessId;

// contains the queue ID of the 'readyList[]' which is stored in Node queueTable[].
// In 'readyList[]' appear only those processes, that are eligible for use later once again.
// In other cases (where state != PROCESS_READY) it shouldn't be placed in on the 'readyList[]'
uint16_t readyList;

uint16_t preemption;

extern uint32_t activeProcessesCounter; // Number of currently active processes

#endif // !_H_GLOBALS