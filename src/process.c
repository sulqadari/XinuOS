#include "xinu.h"

PID32 proc_alloc_process_id(void)
{
    uint32_t i;
    static PID32 nextProcessId = 1;

    for (i = 0; i < QTAB_TOTAL_OF_PROCESSES; ++i)
    {
        nextProcessId %= QTAB_TOTAL_OF_PROCESSES;   // wrap around to beginning
        if (processTable[nextProcessId].state == PROCESS_FREE)
            return nextProcessId++;
        else
            nextProcessId++;
    }

    return SW_FAILED_TO_ALLOCATE_PROCESS_ID;
}