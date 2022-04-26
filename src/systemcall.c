#include "../inc/xinu.h"

PRIO16 resume(PID32 processId)
{
    uint32_t intMask;       // stores interrupt mask
    Process* p_process;
    PRIO16 priority;

    /*To prevent involuntarily relinquishing the processor, a system call have to disable
    * interrupts until a change is complete.*/
    intMask = disable_interrupts();
    if (IS_BAD_PROCESS_ID(processId))
    {
        restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    p_process = &processTable[processId];
    if (PROCESS_SUSPENDED != p_process->state)
    {
        restore_interrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    // store priority of a given process before calling set_ready_state()
    priority = p_process->priority;
    set_ready_state(processId);
    restore_interrupts(intMask);

    return priority;
}

PRIO16 suspend(PID32 processId)
{
    uint32_t intMask;       // stores interrupt mask
    Process* p_process;
    PRIO16 priority;      // priority to return

    intMask = disable_interrupts();
    if (IS_BAD_PROCESS_ID(processId) || (PROCESS_NULL == processId))
    {
        restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    p_process = &processTable[processId];
    if ((PROCESS_CURRENT != p_process->state) && (PROCESS_READY != p_process->state))
    {
        restore_interrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    if (PROCESS_READY == p_process->state)      // are we about to suspend a process which is in ready list?
    {
        get_item(processId);                     // remove a it from the ready list
        p_process->state = PROCESS_SUSPENDED;
    }
    else    // Otherwise: suspend the current process
    {
        p_process->state = PROCESS_SUSPENDED;   // mark the current process as suspended
        reschedule();                    // reschedule processs because we are about to suspend current process.
    }

    priority = p_process->priority;
    // after restoring interrupts "p_process->priority" may be changed
    restore_interrupts(intMask);
    // thus, return the value stored in local copy
    return priority;
}

STATUS kill(PID32 processId)
{
    uint32_t intMask;       // stores interrupt mask
    Process* p_process;
    int32_t i;

    intMask = disable_interrupts();
    p_process = &processTable[processId];
    if (IS_BAD_PROCESS_ID(processId) || (PROCESS_NULL == processId) || (PROCESS_FREE == p_process->state))
    {
        restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    activeProcessesCounter--;
    if (1 >= activeProcessesCounter)    // if the current process happens to be the last process..
        xdone();                        // ..call xdone()
    
    send(p_process->parentProcessId, processId);
    for(i = 0; i < 3; ++i)
        close(p_process->deviceDescriptors[i]);
    
    free_stack(p_process->stackBase, p_process->stackLen);

    switch(p_process->state)
    {
        case PROCESS_CURRENT:
            p_process->state = PROCESS_FREE;
            reschedule();
        // fall through
        case PROCESS_SLEEP:
        case PROCESS_RECEIVING_TMR_OR_MSG:
            unsleep(processId);
            p_process->state = PROCESS_FREE;
        break;
        case PROCESS_WAITING_ON_SEMAPHORE:
            semaphoreTable[p_process->semaphoreId].scount++;
        // fall through
        case PROCESS_READY:
            get_item(processId); //remove from equeue
        // fall through
        default: p_process->state = PROCESS_FREE;
    }
    
    restore_interrupts(intMask);
    return SW_OK;
}

PID32 create(void* funcAddress, uint32_t stackSize, PRIO16 priority, uint8_t* name, uint32_t nargs)
{
    uint32_t savsp;
    uint32_t* pushsp;
    uint32_t intMask;
    PID32 processId;    // stores new process ID
    Process* p_process; // pointer to process table entry
    int8_t i;
    uint32_t* a;        // points to list of args
    uint32_t* saddr;    //stack address

    intMask = disable_interrupts();
    if (stackSize < PROCESS_MIN_STACK_SIZE)
        stackSize = PROCESS_MIN_STACK_SIZE;
    
    stackSize = (uint32_t) round_mb(stackSize);
    processId = get_new_process_id();
    saddr = (uint32_t*) get_stack(stackSize);

    if ((priority < 1) || (processId == SW_FAILED_TO_ALLOCATE_PROCESS_ID))
    {
        restore_interrupts();
        return SW_FAILED_TO_ALLOCATE_PROCESS_ID;
    }

    if (saddr == (uint32_t*) SW_FAILED_TO_ALLOCATE_STACK)
    {
        restore_interrupts();
        return SW_FAILED_TO_ALLOCATE_STACK;
    }

    activeProcessesCounter++;
    p_process = &processTable[processId];

    // initialize process table entry for new process
    p_process->state = PROCESS_SUSPENDED;
    p_process->priority = priority;
    p_process->stackBase = (uint8_t*) saddr;
    p_process->stackLen = stackSize;
    p_process->name[PROCESS_NAME_LENGTH - 1] = PROCESS_NAME_NULL_CHAR;

    do
    {
        p_process->name[i] = name[i];

    }while((i < PROCESS_NAME_LENGTH) && (name[i++] != PROCESS_NAME_NULL_CHAR));

    p_process->semaphoreId = -1;
    p_process->parentProcessId = (PID32)get_process_id();
    p_process->hasMessage = FALSE;

    // set up stdin, stdout and stderr descriptors for the shell
    p_process->deviceDescriptors[0] = CONSOLE;
    p_process->deviceDescriptors[1] = CONSOLE;
    p_process->deviceDescriptors[2] = CONSOLE;

    // Initialize stack as if the process was called
    *saddr = STACK_MAGIC;
    savsp = (uint32_t) saddr;

    // Push arguments
    a = (uint32_t*) (&nargs + 1);   // start of args
    a += (nargs - 1);               // last argument

    for (; nargs > 0; --nargs)      // machine dependent; copy args..
        *--saddr = *a--;            // ..onto created process's stack
    *--saddr = (long) PROCESS_INIT_RETURN;

    /* The following entries on the stack must match what hal_switch_context()
    * expects a saved process state to contain: ret address, ebp, interrupt mask, flags,
    * registers and an old SP (stack pointer)
    */
    
    /*Make the stack look like it's half-way through a call to hal_switch_context()
    * that returns to the new process*/
    *--saddr = (int32_t)funcAddress;
    *--saddr = savsp;               // This will be register ebp for process exit
    savsp = (uint32_t) saddr;       // start of frame from hal_switch_context()
    *--saddr = 0x00000200;          // New process runs with interrupts enabled
    *--saddr = 0;                   // eax
    *--saddr = 0;                   // ecx
    *--saddr = 0;                   // edx
    *--saddr = 0;                   // ebx
    *--saddr = 0;                   // esp; value filled in below
    pushsp = saddr;                 // Remember this location
    *--saddr = savsp;               // ebp (while finishing hal_switch_context())
    *--saddr = 0;                   // esi
    *--saddr = 0;                   // edi

    pushsp = (uint32_t) (p_process->stackPointer = (uint8_t*)saddr);
    restore_interrupts(mask);
    return processId;

}

PID32 get_new_process_id(void)
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

void returnAddress(void)
{
    PID32 processId = get_process_id();
    kill(processId);
}

void xdone(void)
{
    printf("\n\nAll processses have completed.\n\n");
    hal_halt();
}