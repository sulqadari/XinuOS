#ifndef _H_STATUSES
#define _H_STATUSES

#include <stdint.h>

#define SW                                  uint32_t
#define SW_OK                               0x0000
#define SW_BAD_QUEUE_ID                     0x8001
#define SW_BAD_PROCESS_ID                   0x8002
#define SW_BAD_SEMAPHORE_ID                 0x8003
#define SW_QUEUE_TABLE_IS_FULL              0x8004
#define SW_DEFER_HANDLING_EXC               0x8005
#define SW_DEFER_UNKNOWN_CMD_EXC            0x8006
#define SW_BAD_PROCESS_STATE                0x8007
#define SW_BAD_SEMAPHORE_STATE              0x8008
#define SW_FAILED_TO_ALLOCATE_PROCESS_ID    0x8009
#define SW_FAILED_TO_ALLOCATE_STACK         0x800A
#define SW_FAILED_TO_ALLOCATE_SID           0x800B
#define SW_NEGATIVE_SEM_COUNT_VALUE     0x800C


#endif // ! _H_STATUSES