#include "../inc/xinu.h"

void scheduler (void)
{
    Process* oldProcess;  // currently running process, whose data stored in processTable is no longer actual.
    Process* newProcess;

    // whether rescheduling is deferred?
    if (defer.ndefers > 0)
    {
        defer.attempt = DEFER_TRUE; // If so, set attempt variable to TRUE and return to caller function
        return;
    }

    oldProcess = &processTable[currentProcessId];
    if (oldProcess->state == PROCESS_CURRENT) // if process remains eligible...
    {
        if (oldProcess->priority > GET_FIRST_KEY(readyList))        // just return if priority of current process is the biggest one among
            return;                                                 // those stored in readyList. That means that current process must keep working.
        
        oldProcess->state = PROCESS_READY;                          // otherwise: process state should be switched from CURRENT ot READY
        insert(currentProcessId, readyList, oldProcess->priority);  // and inserted at the appropriate index in the readyList
    }

    currentProcessId = remove(readyList);                           // Switch to a process wich has the highest priority
    newProcess = &processTable[currentProcessId];
    newProcess->state = PROCESS_CURRENT;
    preemption = QUANTUM;                                           // reset time slice for process
    ctxsw(&oldProcess->stackPointer, &newProcess->stackPointer);

    return;
}

uint16_t isReschedulingAllowed(uint32_t defRequest)
{
    switch(defRequest)
    {
        case DEFER_START: // handle deferral request
            if (defer.ndefers++ == 0)
                defer.attempt = DEFER_FALSE;
            
            return STATUS_OK;

        case DEFER_STOP: // handle end of deferral
            if (defer.ndefers <= 0)
                return STATUS_DEFER_HANDLING_EXC;
            
            if ((--defer.ndefers == 0) && defer.attempt)
                scheduler();
            return STATUS_OK;

        default: return STATUS_DEFER_UNKNOWN_CMD_EXC;
    }
}