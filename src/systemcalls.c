#include "../inc/xinu.h"

uint16_t resume(int32_t processId)
{
    uint32_t intMask;       // store interrupt mask
    Process* p_process;
    uint16_t priority;

    /*To prevent involuntarily relinquishing the processor, a system call have to disable
    * interrupts until a change is complete.*/
    intMask = disableInterrupts();
    if (IS_BAD_PROCESS_ID(processId))
    {
        restoreInterrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    p_process = &processTable[processId];
    if (p_process->state != PROCESS_SUSPENDED)
    {
        restoreInterrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    // store priority of a given process before calling setReadyState()
    priority = p_process->priority;
    setReadyState(processId);
    restoreInterrupts(intMask);

    // due to fact that after setReadyState() a new process may run,
    // "p_process->priority" as return value is illegal, because we need to return
    // priority of that process which was being unsuspeneded, not that one which
    // is running right now.
    return priority;
}

uint16_t suspend(int32_t processId)
{
    uint32_t intMask;       // saved interrupt mask
    Process* p_process;
    uint16_t priority;      // priority to return

    intMask = disableInterrupts();
    if (IS_BAD_PROCESS_ID(processId) || (processId == PROCESS_NULL))
    {
        restoreInterrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    p_process = &processTable[processId];
    if ((p_process->state != PROCESS_CURRENT) && (p_process->state != PROCESS_READY))
    {
        restoreInterrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    if (p_process->state == PROCESS_READY)      // are we about to suspend a process which is in ready list?
    {
        getItem(processId);                     // remove a it from the ready list
        p_process->state = PROCESS_SUSPENDED;
    }
    else    // Otherwise: suspend the current process
    {
        p_process->state = PROCESS_SUSPENDED;   // mark the current process as suspended
        rescheduleProcess();                    // reschedule processs because we are about to suspend current process.
    }

    priority = p_process->priority;
    // after restoring interrupts "p_process->priority" may be changed
    restoreInterrupts(intMask);
    // thus, return the value stored in local copy
    return priority;
}