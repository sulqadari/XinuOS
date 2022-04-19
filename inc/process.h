#ifndef _H_PROCESS
#define _H_PROCESS

#include <stdint.h>

#define MAX_NUM_OF_ACTIVE_PROCESSES     8   // Maximum number of processes in the system

/* Process state constants */
#define PROCESS_FREE                    0   // unused
#define PROCESS_CURRENT                 1   // running
#define PROCESS_READY                   2   // ready to execute
#define PROCESS_WAITING_MSG             3   // waiting for message
#define PROCESS_SLEEP                   4   // waiting for timer (sleeping)
#define PROCESS_SUSPEND                 5   // suspended
#define PROCESS_WAITING_ON_SEMAPHORE    6   // semaphore queue (waiting for semaphore)
#define PROCESS_RECEIVING_TMR_MSG       7   // waiting for a timer or a message (whichever occur first)

#define PROCESS_NAME_LENGTH             16
#define PROCESS_NULL                    0   // Perhaps this constant is redundant: PROCESS_FREE constant has a similar sense

/* process initialization constants */
#define PROCESS_STACK_SIZE              0xFFFF          // initial process size
#define PROCESS_INIT_PRIORITY           20              // initial process priority
#define PROCESS_INIT_RETURN             returnAddress   // an address to which process returns
#define STACK_MAGIC                     0xCAFEBABE      // marker for the top of a process stack (used to help detect overflow)

// Number of device descriptors a process can have open.
// Must be odd to make 'Process' 4N bytes (whatever that means...).
#define NUMBER_OF_OPEN_DEVICE_DESCRIPTORS    5   

typedef struct ProcessTableEntry
{
    uint16_t state;                     // stores state info
    uint16_t priority;                  // 
    uint8_t* stackPointer;
    uint8_t* stackBase;
    uint32_t stackLen;
    uint8_t name[PROCESS_NAME_LENGTH];
    uint32_t semaphoreId;
    uint32_t parentProcess;
    uint32_t receivedMessage;
    uint8_t hasMessage;     // non-zero if message is valid
    uint16_t deviceDescriptors[NUMBER_OF_OPEN_DEVICE_DESCRIPTORS];    // Device descriptors for process
} Process;

extern Process processTable[];

// inline code to check process ID (assumes)
#define IS_BAD_PROCESS_ID(x)    (   ((int32_t)(x) < 0)\
                                || (((uint32_t)(x) >= MAX_NUM_OF_ACTIVE_PROCESSES))\
                                || (processTable[(x)].state == PROCESS_FREE))

#endif // !_H_PROCESS
