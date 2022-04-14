#ifndef _H_PROCESS
#define _H_PROCESS

#include <stdint.h>

// max number of processes in the system
#define MAX_NUM_OF_ACTIVE_PROCESSES     8
#define PROCESS_FREE                    0   // is unused
#define PROCESS_CURRENT                 1   // running
#define PROCESS_READY                   2   // ready to execute
#define PROCESS_WAITING_MSG             3   // waiting message
#define PROCESS_SLEEP                   4   // sleeping
#define PROCESS_SUSPEND                 5   // suspended
#define PROCESS_WAITING_ON_SEMAPHORE    6   // semaphore queue
#define PROCESS_RECEIVING_TMR_MSG       7   // waiting for a timer or a message

#define PROCESS_NAME_LENGTH             16
#define PROCESS_NULL                    0

/* process initialization constants */
#define PROCESS_STACK_SIZE              0xFFFF      // initial process size
#define PROCESS_INIT_PRIORITY           20          // initial process priority
#define PROCESS_INIT_RETURN             0x8000      // address to which process returns
#define STACK_MAGIC                     0xCAFEBABE  // marker for the top of a process stack (used to help detect overflow)

// Number of device descriptors a process can have open.
// must be odd to make processEntry 4N bytes
#define NUMBER_OF_DEVICE_DESCRIPTORS    5   

typedef struct ProcessTableEntry
{
    uint16_t state;         // 
    int16_t priority;
    int8_t* stackPointer;
    int8_t* stackBase;
    uint32_t stackLen;
    int8_t name[PROCESS_NAME_LENGTH];
    int32_t semaphoreId;
    int32_t parentProcess;
    int32_t receivedMessage;
    uint8_t hasMessage;     // non-zero if message is valid
    int16_t deviceDescriptors[NUMBER_OF_DEVICE_DESCRIPTORS];    // Device descriptors for process
} Process;

extern Process processTable[];
extern int32_t processCounter;
extern int32_t currentProcessId;

// inline code to check process ID (assumes)
#define IS_BAD_PROCESS_ID(x)    ( ((int32_t)(x) < 0) || (((int32_t)(x) >= MAX_NUM_OF_ACTIVE_PROCESSES)) || (processTable[(x)].processState == PROCESS_FREE))

#endif // !_H_PROCESS
