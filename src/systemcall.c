#include "../inc/xinu.h"

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
    if (PROCESS_SUSPENDED != p_process->state)
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    // see chapter 6.7 "System call return values SYSERR and OK"
    priority = p_process->priority;
    scheduler_set_ready_state(processId);
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
    if ((PROCESS_CURRENT != p_process->state) && (PROCESS_READY != p_process->state))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    if (PROCESS_READY == p_process->state)      // are we about to suspend a process which is in ready list?
    {
        q_get_item(processId);                  // remove a it from the ready list
        p_process->state = PROCESS_SUSPENDED;
    }
    else    // Otherwise: syscall_suspend the current process
    {
        p_process->state = PROCESS_SUSPENDED;   // mark the current process as suspended
        reschedule();                           // reschedule processs because we are about to suspend current process.
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
    if (IS_BAD_PROCESS_ID(processId) || (PROCESS_NULL == processId) || (PROCESS_FREE == p_process->state))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    activeProcessesCounter--;
    if (1 >= activeProcessesCounter)    // if the current process happens to be the last one..
        syscall_xdone();                        // ..call syscall_xdone()
    
    syscall_send(p_process->parentProcessId, processId);        //!!! TODO: requires implementation
    for(i = 0; i < 3; ++i)
        syscall_close(p_process->deviceDescriptors[i]);         ///!!! TODO: requires implementation
    
    hal_free_stack(p_process->stackBase, p_process->stackLen);

    switch(p_process->state)
    {
        case PROCESS_CURRENT:
            p_process->state = PROCESS_FREE;
            reschedule();
        // fall through
        case PROCESS_SLEEP:
        case PROCESS_RECEIVING_TMR_OR_MSG:
            proc_unsleep(processId);                     //!!! TODO: requires implementation
            p_process->state = PROCESS_FREE;
        break;
        case PROCESS_WAITING_ON_SEMAPHORE:
            semaphoreTable[p_process->semaphoreId].scount++;
        // fall through
        case PROCESS_READY:
            q_get_item(processId); //q_remove from equeue
        // fall through
        default: p_process->state = PROCESS_FREE;
    }
    
    hal_restore_interrupts(intMask);
    return SW_OK;
}

PID32 syscall_create_process(void* funcAddress, uint32_t stackSize, PRIO16 priority, uint8_t* name, uint32_t nargs)
{
    uint32_t savsp;
    uint32_t* pushsp;
    INTMASK intMask;
    PID32 processId;            // stores new process ID
    Process* p_process;         // pointer to process table entry
    int8_t i;
    uint32_t* a;                // points to list of args
    uint32_t* stackAddress;    //stack address

    intMask = hal_disable_interrupts();
    if (stackSize < PROCESS_MIN_STACK_SIZE)
        stackSize = PROCESS_MIN_STACK_SIZE;
    
    stackSize = (uint32_t) syscall_round_mb(stackSize);
    processId = proc_alloc_process_id();
    stackAddress = hal_alloc_stack(stackSize);

    if ((priority < 1) || (processId == SW_FAILED_TO_ALLOCATE_PROCESS_ID))
    {
        hal_restore_interrupts();
        return SW_FAILED_TO_ALLOCATE_PROCESS_ID;
    }

    if (stackAddress == (uint32_t*) SW_FAILED_TO_ALLOCATE_STACK)
    {
        hal_restore_interrupts();
        return SW_FAILED_TO_ALLOCATE_STACK;
    }

    activeProcessesCounter++;
    p_process = &processTable[processId];

    // initialize process table entry for new process
    p_process->state = PROCESS_SUSPENDED;
    p_process->priority = priority;
    p_process->stackBase = (uint8_t*) stackAddress;
    p_process->stackLen = stackSize;
    p_process->name[PROCESS_NAME_LENGTH - 1] = PROCESS_NAME_NULL_CHAR;

    i = 0;
    do
    {
        p_process->name[i] = name[i];

    }while((i < (PROCESS_NAME_LENGTH - 1)) && (name[i++] != PROCESS_NAME_NULL_CHAR));

    p_process->semaphoreId = -1;
    p_process->parentProcessId = (PID32)syscall_get_process_id();
    p_process->hasMessage = FALSE;

    // set up stdin, stdout and stderr descriptors for the shell
    p_process->deviceDescriptors[0] = CONSOLE;
    p_process->deviceDescriptors[1] = CONSOLE;
    p_process->deviceDescriptors[2] = CONSOLE;

    // Initialize stack as if the process was called

    *stackAddress = STACK_MAGIC;        // stack's upper bound
    savsp = (uint32_t) stackAddress;    // store stack address as integer value

    // TODO: I expect that the code below should be moved to this function
    hal_create_stack_image();

    // Push arguments
    a = (uint32_t*) (&nargs + 1);           // start of args
    a += (nargs - 1);                       // last argument

    for (; nargs > 0; --nargs)              // machine dependent; copy args..
        *--stackAddress = *a--;             // ..onto created process's stack
    
    *--stackAddress = (long) PROCESS_INIT_RETURN;   // push on return address

    /* The following entries on the stack must match what hal_switch_context()
    * expects a saved process state to contain: ret address, ebp, interrupt mask, flags,
    * registers and an old SP (stack pointer)
    */
    
    /*Make the stack look like it's half-way through a call to hal_switch_context()
    * that returns to the new process*/
    *--stackAddress = (int32_t)funcAddress;
    *--stackAddress = savsp;               // This will be register ebp for process exit
    savsp = (uint32_t) stackAddress;       // start of frame from hal_switch_context()
    *--stackAddress = 0x00000200;          // New process runs with interrupts enabled
    *--stackAddress = 0;                   // eax
    *--stackAddress = 0;                   // ecx
    *--stackAddress = 0;                   // edx
    *--stackAddress = 0;                   // ebx
    *--stackAddress = 0;                   // esp; value filled in below
    pushsp = stackAddress;                 // Remember this location
    *--stackAddress = savsp;               // ebp (while finishing hal_switch_context())
    *--stackAddress = 0;                   // esi
    *--stackAddress = 0;                   // edi

    p_process->stackPointer = (uint8_t*)stackAddress;
    *pushsp = (uint32_t)p_process->stackPointer;
    hal_restore_interrupts(intMask);
    return processId;

}

void syscall_return_address(void)
{
    PID32 processId = syscall_get_process_id();
    syscall_kill(processId);
}

void syscall_xdone(void)
{
    printf("\n\nAll processses have completed.\n\n");
    hal_halt();
}

PRIO16 syscall_get_process_priority(PID32 processId)
{
    INTMASK intMask;
    PRIO16 priority;

    intMask = hal_disable_interrupts();
    if (IS_BAD_PROCESS_ID(processId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    priority = processTable[processId].priority;
    hal_restore_interrupts(intMask);
    return priority;
}

PID32 syscall_get_process_id(void)
{
    return currentProcessId;
}

PRIO16 syscall_change_priority(PID32 processId, PRIO16 newPriority)
{
    INTMASK intMask;
    Process* p_process;
    PRIO16 oldPriority;

    intMask = hal_disable_interrupts();
    if (IS_BAD_PROCESS_ID(processId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    p_process = &processTable[processId];
    oldPriority = p_process->priority;
    p_process->priority = newPriority;

    hal_restore_interrupts(intMask);
    return oldPriority;
}