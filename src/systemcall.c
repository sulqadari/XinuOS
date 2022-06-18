#include "../inc/xinu.h"

PID32 syscall_create_process(void* funcAddress, uint32_t stackSize, PRIO16 priority, uint8_t* name, uint32_t nargs, ...)
{
    uint32_t savsp;
    uint32_t* pushsp;
    INTMASK intMask;
    PID32 processId;            // stores new process ID
    Process* p_process;         // pointer to process table entry
    int8_t i;
    uint32_t* p_argsList;       // points to list of args
    uint32_t* stackAddress;    //stack address

    intMask = hal_disable_interrupts();

    if (stackSize < PROCESS_MIN_STACK_SIZE)
        stackSize = PROCESS_MIN_STACK_SIZE;
    
    stackSize = (uint32_t) mem_round_mb(stackSize);
    
    processId = proc_alloc_process_id();
    stackAddress = mem_alloc_stack(stackSize);

    if ((priority < 1) || (SW_FAILED_TO_ALLOCATE_PROCESS_ID == processId))
    {
        hal_restore_interrupts(intMask);
        return SW_FAILED_TO_ALLOCATE_PROCESS_ID;
    }

    if ((uint32_t*) SW_FAILED_TO_ALLOCATE_STACK == stackAddress)
    {
        hal_restore_interrupts(intMask);
        return SW_FAILED_TO_ALLOCATE_STACK;
    }

    activeProcessesCounter++;
    p_process = &processTable[processId];

    // initialize process table entry for new process
    p_process->state = PROCESS_STATE_SUSPENDED;
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

    *stackAddress = PROCESS_STACK_MAGIC;        // stack's upper bound
    savsp = (uint32_t) stackAddress;    // store stack address as integer value

    // TODO: I expect that the code below should be moved to this function
    hal_create_stack_image();

    // Push arguments
    p_argsList = (uint32_t*) (&nargs + 1);           // start of args
    p_argsList += (nargs - 1);                       // last argument

    for (; nargs > 0; --nargs)              // machine dependent; copy args..
        *--stackAddress = *p_argsList--;             // ..onto created process's stack
    
    *--stackAddress = (long) PROCESS_INIT_RETURN;   // push on return address

    /* The following entries on the stack must match what proc_switch_context()
    * expects a saved process state to contain: ret address, ebp, interrupt mask, flags,
    * registers and an old SP (stack pointer)
    */
    
    /*Make the stack look like it's half-way through a call to proc_switch_context()
    * that returns to the new process*/
    *--stackAddress = (int32_t)funcAddress;
    *--stackAddress = savsp;               // This will be register ebp for process exit
    savsp = (uint32_t) stackAddress;       // start of frame from proc_switch_context()
    *--stackAddress = 0x00000200;          // New process runs with interrupts enabled
    *--stackAddress = 0;                   // eax
    *--stackAddress = 0;                   // ecx
    *--stackAddress = 0;                   // edx
    *--stackAddress = 0;                   // ebx
    *--stackAddress = 0;                   // esp; value filled in below
    pushsp = stackAddress;                 // Remember this location
    *--stackAddress = savsp;               // ebp (while finishing proc_switch_context())
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

PRIO16 syscall_set_process_priority(PID32 processId, PRIO16 newPriority)

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

PID32 syscall_get_process_id(void)
{
    return currentProcessId;
}

SW syscall_send_msg(PID32 processId, uMSG32 msg)
{
    INTMASK intMask;
    Process* p_process;
    intMask = hal_disable_interrupts();

    if (IS_BAD_PROCESS_ID(processId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_ID;
    }

    p_process = &processTable[processId];
    if (PROCESS_STATE_FREE == p_process->state)
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_PROCESS_STATE;
    }

    if (p_process->hasMessage)
    {
        hal_restore_interrupts(intMask);
        return SW_PROCESS_OUTSTANDING_MSG_EXC;
    }

    p_process->hasMessage = msg;
    p_process->hasMessage = TRUE;

    // If recipient waiting or in timed-wait make it ready
    if (PROCESS_STATE_WAITING_MSG == p_process->state)
    {
        proc_set_ready_state(processId);
    }
    else if (PROCESS_STATE_RECEIVING_TMR_OR_MSG == p_process->state)
    {
        proc_unsleep(processId);
        proc_set_ready_state(processId);
    }

    hal_restore_interrupts(intMask);
    return SW_OK;
}

uMSG32 syscall_receive_msg(void)
{
    INTMASK intMask;
    Process* p_process;
    uMSG32 msg;

    intMask = hal_disable_interrupts();
    p_process = &processTable[currentProcessId];

    if (FALSE == p_process->hasMessage)
    {
        p_process->state = PROCESS_STATE_WAITING_MSG;
        proc_reschedule();   // Block until message arrives
    }

    msg = p_process->receivedMessage;
    p_process->hasMessage = FALSE;
    hal_restore_interrupts(intMask);
    return msg;
}

uMSG32 syscall_nb_receive_msg(void)
{
    INTMASK intMask;
    Process* p_process;
    uMSG32 msg;

    intMask = hal_disable_interrupts();
    p_process = &processTable[currentProcessId];

    if (TRUE == p_process->hasMessage)
    {
        msg = p_process->receivedMessage;
        p_process->hasMessage = FALSE;
    }
    else
    {
        msg = SW_OK;
    }

    hal_restore_interrupts(intMask);
    return msg;
}
