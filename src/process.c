#include "../inc/xinu.h"

uint16_t setReadyState(uint32_t processId)
{
    Process* p_process;
    
    if (IS_BAD_PROCESS_ID(processId))
        return STATUS_BAD_PROCESS_ID;
    
    p_process = &processTable[processId];
    p_process->state = PROCESS_READY;

    insert(processId, readyList, p_process->priority);
    rescheduleProcess();

    return STATUS_OK;
}