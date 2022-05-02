#ifndef _H_SEMAPHORE
#define _H_SEMAPHORE

#include <stdint.h>
#include "globals.h"

#define NUMBER_OF_SEMAPHORES    120
#define SEMAPHORE_FREE          0
#define SEMAPHORE_USED          1

typedef struct struct_Semaphore
{
    int8_t state;
    int32_t count;
    QID16 queueId;
} Semaphore;

extern Semaphore semaphoreTable[];

#define IS_BAD_SEMAPHORE(S) (((int32_t)(S) < 0) || ((S) >= NUMBER_OF_SEMAPHORES))
#endif // !_H_SEMAPHORE