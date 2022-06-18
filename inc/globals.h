#ifndef _H_GLOBALS
#define _H_GLOBALS

#include <stdint.h>
#include "status.h"

//--------------------------------------------------------------//
// Project-specific definitions and data types                  //
//--------------------------------------------------------------//
#define TRUE    1
#define FALSE   0
#define CONSOLE 1

#define PID32   int32_t     // Process ID
#define QID16   int16_t     // Queue ID
#define KID32   int32_t     // Key ID
#define SID32   int32_t     // Semaphore ID
#define PRIO16  int16_t     // Process priority
#define uMSG32  uint32_t    // Message
#define STATUS  int16_t     // status
#define INTMASK uint32_t    // mask of interruptions
#define STACK   uint32_t*   // stack size



//--------------------------------------------------------------//
// queue.h-specific definitions and data types                  //
//--------------------------------------------------------------//
/* Default number of queue entries:
 * 1 per process +
 * 2 ready list +
 * 2 sleep list +
 * 2 semaphore */
#define QTAB_TOTAL_ENTRIES    (ACTIVE_PROCESSES + 4 + SEM_TOTAL + SEM_TOTAL)        //NQENT (NPROC + 4 + SEM_TOTAL + SEM_TOTAL)
#define QTAB_EMPTY              (-1)
#define QTAB_MAX_KEY            0x7FFFFFFF
#define QTAB_MIN_KEY            0x80000000

typedef struct ListEntry
{
    KID32 key;
    QID16 next;
    QID16 previous;
} Node;

/* Contains QTAB_TOTAL_ENTRIES entries. Represents a doubly linked list data structure.
 * An important implicit boundary occurs between element ACTIVE_PROCESSES - 1 and ACTIVE_PROCESSES.
 * Each element below the boundary corresponds to a process ID,
 * and the elements queueTable[ACTIVE_PROCESSES] through queueTable[QTAB_TOTAL_ENTRIES]
 * correspond to the heads or tails of lists.*/
extern Node queueTable[];

// contains the queue ID of the 'readyList[]' which is stored in Node queueTable[].
// In 'readyList[]' appear only those processes, that are eligible for use later once again.
// In other cases (where state != PROCESS_STATE_READY) it shouldn't be placed in on the 'readyList[]'
extern int16_t readyList;



//--------------------------------------------------------------//
// process.h-specific definitions and data types                //
//--------------------------------------------------------------//
// Maximum number of processes in the system
#define ACTIVE_PROCESSES 8   

/* Process state constants */
#define PROCESS_STATE_FREE                  0   // unused
#define PROCESS_STATE_CURRENT               1   // running
#define PROCESS_STATE_READY                 2   // ready to execute
#define PROCESS_STATE_WAITING_MSG           3   // waiting for message
#define PROCESS_STATE_SLEEP                 4   // waiting for timer (sleeping)
#define PROCESS_STATE_SUSPENDED             5   // suspended
#define PROCESS_STATE_WAITING_ON_SEMAPHORE  6   // semaphore queue (waiting for semaphore)
#define PROCESS_STATE_RECEIVING_TMR_OR_MSG  7   // waiting for a timer or a message (whichever occurs first)

#define PROCESS_NAME_LENGTH     16
#define PROCESS_NAME_NULL_CHAR  '\0'
#define PROCESS_NULL            0

/* process initialization constants */
#define PROCESS_STACK_SIZE      ((uint16_t)0xFFFF)      // initial process stack size
#define PROCESS_MIN_STACK_SIZE  0x1000
#define PROCESS_INIT_PRIORITY   20                      // initial process priority
#define PROCESS_INIT_RETURN     syscall_return_address  // an address to which process returns
#define PROCESS_STACK_MAGIC     0xCAFEBABE              // marker for the top of a process stack (used to help detect overflow)

// Number of device descriptors a process can have open.
// Must be odd to make 'Process' 4N bytes (whatever that means...).
#define DEVICE_DESCRIPTORS  5

#define DEFER_TRUE  1
#define DEFER_FALSE 0
#define QUANTUM     0xA5

#define DEFER_START 1
#define DEFER_STOP  0

typedef struct ProcessTableEntry
{
    uint16_t state;
    PRIO16 priority;
    uint8_t* stackPointer;
    uint8_t* stackBase;
    uint32_t stackLen;
    uint8_t name[PROCESS_NAME_LENGTH];
    SID32 semaphoreId;
    PID32 parentProcessId;
    uMSG32 receivedMessage;
    uint8_t hasMessage;     // non-zero if message is valid
    uint8_t deviceDescriptors[DEVICE_DESCRIPTORS];    // Device descriptors for process
} Process;

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

// stores ID of currently running process.
// Current process does not appear on the ready list. To provide fast 
// access to the current process, its ID is stored in this variable.
extern PID32 currentProcessId;
extern int16_t preemption;
extern int32_t activeProcessesCounter; // Number of currently active processes
extern Process processTable[];
extern Defer defer;                     // Global counter of requests to defer rescheduling.

//--------------------------------------------------------------//
// semaphore.h-specific definitions and data types              //
//--------------------------------------------------------------//
#define SEM_TOTAL       120 // default number of semaphores
#define SEMAPHORE_FREE   0   // semaphore table entry is available
#define SEMAPHORE_BUSY   1   // semaphore table entry is in use

/**
 * @brief  Semaphore table entry
 * @note   
 * @retval None
 */
typedef struct SemaphoreTableEntry
{
    int8_t state;   // informs whether the entry is currently used (i.e. allocated) or free
    int32_t count;  // if has a negative value then associated processes are blocked
    QID16 queueId;  // index of the head of the list of processes that are waiting on the semaphore
} Semaphore;

extern Semaphore semaphoreTable[];

#endif // !_H_GLOBALS