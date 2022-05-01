#include "../inc/xinu.h"

void reschedule(void)
{
    Process* currProcess;  // currently running process, whose data stored in processTable is no longer actual.
    Process* nextProcess;

    // whether rescheduling is deferred?
    if (defer.defersCounter > 0)
    {
        defer.attempt = DEFER_TRUE; // If so, set attempt variable to TRUE and return to caller function
        return;
    }

    // Before current process calls reschedule() it may change its own state..
    currProcess = &processTable[currentProcessId];
    if (currProcess->state == PROCESS_CURRENT) // ..thus, if process left its state as eligible..
    {
        if (currProcess->priority > GET_FIRST_KEY(readyList))        // ..just return if priority of current process is the biggest one among
            return;                                                  // those stored in readyList[]. Which means current process must keep working.
        
        currProcess->state = PROCESS_READY;                          // otherwise: process's state should be switched from CURRENT ot READY
        q_insert(currentProcessId, readyList, currProcess->priority);  // and process whould be inserted at the appropriate index in the readyList[]
    }

    // As mentioned in '5.6 Implementation of scheduling' current implementation assumes
    // that current process should not appear on the readyList[].
    currentProcessId = q_remove(readyList);
    nextProcess = &processTable[currentProcessId];                          // Switch to a process wich has the highest priority
    nextProcess->state = PROCESS_CURRENT;                                   // Mark current process as 'CURRENT'
    preemption = QUANTUM;                                                   // reset time slice for process
    hal_switch_context(currProcess->stackPointer, nextProcess->stackPointer);  // save hardware registers of the current process.

    return;
}

STATUS reschedule_control(int32_t defReq)
{
    switch(defReq)
    {
        case DEFER_START: // handle deferral request
        {
            defer.defersCounter++;
            if (defer.defersCounter == 0)
                defer.attempt = DEFER_FALSE;
            
            return SW_OK;
        }break;
        case DEFER_STOP: // handle end of deferral
        {
            if (defer.defersCounter <= 0)
                return SW_DEFER_HANDLING_EXC;
            
            --defer.defersCounter;
            if ((defer.defersCounter == 0) && defer.attempt)
                reschedule();
            
            return SW_OK;
        }break;
        default: return SW_DEFER_UNKNOWN_CMD_EXC;
    }
}

STATUS scheduler_set_ready_state(PID32 processId)
{
    Process* p_process;

    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;

    p_process = &processTable[processId];
    p_process->state = PROCESS_READY;

    q_insert(processId, readyList, p_process->priority);
    reschedule();

    return SW_OK;
}