#include "../inc/xinu.h"

int16_t setReadyState(int32_t processId)
{
    Process* p_process;

    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;

    p_process = &processTable[processId];
    p_process->state = PROCESS_READY;

    insert(processId, readyList, p_process->priority);
    rescheduleProcess();

    return SW_OK;
}