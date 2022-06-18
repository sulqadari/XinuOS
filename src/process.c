#include "../inc/xinu.h"

Defer defer;

void proc_reschedule(void)
{
    Process* currProcess;  // currently running process, whose data stored in processTable is no longer actual.
    Process* nextProcess;

    // whether rescheduling is deferred?
    if (defer.defersCounter > 0)
    {
        defer.attempt = DEFER_TRUE; // If so, set attempt variable to TRUE and return to caller function
        return;
    }

    // Before current process calls proc_reschedule() it may change its own state..
    currProcess = &processTable[currentProcessId];
    if (PROCESS_STATE_CURRENT == currProcess->state) // ..thus, if process left its state as eligible..
    {
        if (GET_FIRST_KEY(readyList) > currProcess->priority)           // ..just return if priority of current process is the biggest one among
            return;                                                     // those stored in readyList[]. Which means current process must keep working.
        
        currProcess->state = PROCESS_STATE_READY;                       // otherwise: process's state should be switched from CURRENT ot READY
        q_insert(currentProcessId, readyList, currProcess->priority);   // and process whould be inserted at the appropriate index in the readyList[]
    }

    // As mentioned in '5.6 Implementation of scheduling' current implementation assumes
    // that current process should not appear on the readyList[].
    currentProcessId = q_remove(readyList);
    nextProcess = &processTable[currentProcessId];                              // Switch to a process wich has the highest priority
    nextProcess->state = PROCESS_STATE_CURRENT;                                 // Mark current process as 'CURRENT'
    preemption = QUANTUM;                                                       // reset time slice for process
    proc_switch_context(currProcess->stackPointer, nextProcess->stackPointer);   // save hardware registers of the current process.

    return;
}

SW proc_defer_handler(int32_t defReq)
{
    switch(defReq)
    {
        case DEFER_START: // handle deferral request
        {
            if (0 == defer.defersCounter)
                defer.attempt = DEFER_FALSE;
            
            defer.defersCounter++;
            return SW_OK;
        }break;
        case DEFER_STOP: // handle end of deferral
        {
            if (defer.defersCounter <= 0)
                return SW_DEFER_HANDLING_EXC;
            
            defer.defersCounter--;
            if ((0 == defer.defersCounter) && defer.attempt)
                proc_reschedule();
            
            return SW_OK;
        }break;
        default: return SW_DEFER_UNKNOWN_CMD_EXC;
    }
}

void proc_switch_context(uint8_t* currStackPtr, uint8_t* nextStackPtr)
{
    ///TODO: implementation have to be transferred to HAL level
}

SW proc_set_ready_state(PID32 processId)
{
    register Process* p_process;

    if (IS_BAD_PROCESS_ID(processId))
        return SW_BAD_PROCESS_ID;

    p_process = &processTable[processId];
    p_process->state = PROCESS_STATE_READY;

    q_insert(processId, readyList, p_process->priority);
    proc_reschedule();

    return SW_OK;
}

PID32 proc_alloc_process_id(void)
{
    uint32_t i;
    static PID32 nextProcessId = 1;

    for (i = 0; i < QTAB_TOTAL_ENTRIES; ++i)
    {
        nextProcessId %= QTAB_TOTAL_ENTRIES;   // wrap around to beginning
        if (processTable[nextProcessId].state == PROCESS_STATE_FREE)
            return nextProcessId++;
        else
            nextProcessId++;
    }

    return SW_FAILED_TO_ALLOCATE_PROCESS_ID;
}

PRIO16 syscall_resume(PID32 processId)
{
    INTMASK intMask;       // stores interrupt mask
    Process* p_process;
    PRIO16 priority;

    /*To prevent involuntarily relinquishing the processor, a system call have to disable
    * interrupts until a change is complete.*/
    intMask = hal_disable_interrupts();
    if (IS_BAD_PROCESS_ID(processId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    p_process = &processTable[processId];
    if (PROCESS_STATE_SUSPENDED != p_process->state)
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    // see chapter 6.7 "System call return values SYSERR and OK"
    priority = p_process->priority;
    proc_set_ready_state(processId);
    hal_restore_interrupts(intMask);

    return priority;
}

PRIO16 syscall_suspend(PID32 processId)
{
    INTMASK intMask;       // stores interrupt mask
    Process* p_process;
    PRIO16 priority;      // priority to return

    intMask = hal_disable_interrupts();
    if (IS_BAD_PROCESS_ID(processId) || (PROCESS_NULL == processId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    p_process = &processTable[processId];
    if ((PROCESS_STATE_CURRENT != p_process->state) && (PROCESS_STATE_READY != p_process->state))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    if (PROCESS_STATE_READY == p_process->state)      // are we about to suspend a process which is in ready list?
    {
        q_get_item(processId);                  // remove a it from the ready list
        p_process->state = PROCESS_STATE_SUSPENDED;
    }
    else    // Otherwise: syscall_suspend the current process
    {
        p_process->state = PROCESS_STATE_SUSPENDED;   // mark the current process as suspended
        proc_reschedule();                           // reschedule processs because we are about to suspend current process.
    }

    // see chapter 6.10 "The value returned by suspend" for details
    priority = p_process->priority;
    hal_restore_interrupts(intMask);
    return priority;
}

STATUS syscall_kill(PID32 processId)
{
    INTMASK intMask;       // stores interrupt mask
    Process* p_process;
    int32_t i;

    intMask = hal_disable_interrupts();
    p_process = &processTable[processId];
    if (IS_BAD_PROCESS_ID(processId) || (PROCESS_NULL == processId) || (PROCESS_STATE_FREE == p_process->state))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    activeProcessesCounter--;
    if (1 >= activeProcessesCounter)    // if the current process happens to be the last one..
        syscall_xdone();                        // ..call syscall_xdone()
    
    syscall_send(p_process->parentProcessId, processId);        //!!! TODO: requires implementation
    for(i = 0; i < 3; ++i)
        device_close(p_process->deviceDescriptors[i]);         ///!!! TODO: requires implementation
    
    mem_free_stack(p_process->stackBase, p_process->stackLen);

    switch(p_process->state)
    {
        case PROCESS_STATE_CURRENT:
            p_process->state = PROCESS_STATE_FREE;
            proc_reschedule();
        // fall through
        case PROCESS_STATE_SLEEP:
        case PROCESS_STATE_RECEIVING_TMR_OR_MSG:
            proc_unsleep(processId);                     //!!! TODO: requires implementation
            p_process->state = PROCESS_STATE_FREE;
        break;
        case PROCESS_STATE_WAITING_ON_SEMAPHORE:
            semaphoreTable[p_process->semaphoreId].count++;
        // fall through
        case PROCESS_STATE_READY:
            q_get_item(processId); //q_remove from equeue
        // fall through
        default: p_process->state = PROCESS_STATE_FREE;
    }
    
    hal_restore_interrupts(intMask);
    return SW_OK;
}

void syscall_xdone(void)
{
    printf("\n\nAll processses have completed.\n\n");
    hal_halt();
}