#ifndef _H_GLOBALS
#define _H_GLOBALS

#include <stdint.h>

/* Project specific data types */
#define TRUE    1
#define FALSE   0
#define CONSOLE 1

#define PID32   int32_t     //Process ID
#define QID16   int16_t     //Queue ID
#define KID32   int32_t     //Key ID
#define SID32   int32_t     //Semaphore ID
#define PRIO16  int16_t     //Process priority
#define uMSG32  uint32_t    // Message
#define STATUS  int16_t     //status

/**
 * @brief  Global requests counter to defer rescheduling.
 * @note   
 * @retval None
 */
typedef struct DeferralControl
{
    int16_t defersCounter;
    int8_t attempt;
} Defer;

// Global counter of requests to defer rescheduling.
extern Defer defer;

// stores ID of currently running process.
// Current process does not appear on the ready list. To provide fast 
// access to the current process, its ID is stored in this variable.
extern PID32 currentProcessId;

// contains the queue ID of the 'readyList[]' which is stored in Node queueTable[].
// In 'readyList[]' appear only those processes, that are eligible for use later once again.
// In other cases (where state != PROCESS_READY) it shouldn't be placed in on the 'readyList[]'
extern int16_t readyList;

extern int16_t preemption;

extern int32_t activeProcessesCounter; // Number of currently active processes

#endif // !_H_GLOBALS