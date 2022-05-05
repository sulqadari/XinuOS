#include "../inc/xinu.h"

SW sem_wait(SID32 semaphoreId)
{
    INTMASK intMask;
    Process* p_process;
    Semaphore* p_semaphore;

    intMask = hal_disable_interrupts();

    if (IS_BAD_SEMAPHORE_ID(semaphoreId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_SEMAPHORE_ID;
    }

    p_semaphore = &semaphoreTable[semaphoreId];
    
    if (SEMAPHORE_FREE == p_semaphore->state)
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_SEMAPHORE_STATE;
    }

    p_semaphore->count--;

    if (p_semaphore->count < 0) // If semaphore counter is negative..
    {
        p_process = &processTable[currentProcessId];        //..get current process..
        p_process->state = PROCESS_WAITING_ON_SEMAPHORE;    //..set its state to WAITING..
        p_process->semaphoreId = semaphoreId;               //..record semaphore ID associated with it..
        q_add(currentProcessId, p_semaphore->queueId);      //..and enqueue current process on semaphore list
        reschedule();                                       //..switch to next process
    }
    
    hal_restore_interrupts(intMask);
    return SW_OK;
}

SW sem_signal(SID32 semaphoreId)
{
    INTMASK intMask;
    PID32 processId;        //..process to be moved from semaphore waiting list to ready list
    Semaphore* p_semaphore;

    intMask = hal_disable_interrupts();

    if (IS_BAD_SEMAPHORE_ID(semaphoreId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_SEMAPHORE_ID;
    }

    p_semaphore = &semaphoreTable[semaphoreId];

    if (SEMAPHORE_FREE == p_semaphore->state)
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_SEMAPHORE_STATE;
    }

    p_semaphore->count++;

    if (p_semaphore->count < 0) // release a waiting process
    {
        processId = q_remove(p_semaphore->queueId);
        scheduler_set_ready_state(processId);
    }

    hal_restore_interrupts(intMask);
    return SW_OK;
}

SID32 sem_create(int32_t count)
{
    INTMASK intMask;
    SID32 semaphoreId;

    intMask = hal_disable_interrupts();
    
    if (count < 0)
    {
        hal_restore_interrupts(intMask);
        return SW_NEGATIVE_SEM_COUNT_VALUE;
    }

    semaphoreId = sem_get_new_sid();
    if (SW_FAILED_TO_ALLOCATE_SID == semaphoreId)
    {
        hal_restore_interrupts(intMask);
        return SW_FAILED_TO_ALLOCATE_SID;
    }

    semaphoreTable[semaphoreId].count = count;
    hal_restore_interrupts(intMask);
    return SW_OK;
}

SID32 sem_get_new_sid(void)
{
    static SID32 nextSemaphore = 0;
    SID32 semaphoreId;
    int32_t i;

    for (i = 0; i < NUMBER_OF_SEMAPHORES; ++i)
    {
        semaphoreId = nextSemaphore++;
        
        if (nextSemaphore >= NUMBER_OF_SEMAPHORES)
            nextSemaphore = 0;
        
        if (SEMAPHORE_FREE == semaphoreTable[semaphoreId].state)
        {
            semaphoreTable[semaphoreId].state = SEMAPHORE_USED;
            return semaphoreId;
        }
    }

    return SW_FAILED_TO_ALLOCATE_SID;
}

SW sem_delete(SID32 semaphoreId)
{
    INTMASK intMask;
    PID32 processId;
    Semaphore* p_semaphore;

    intMask = hal_disable_interrupts();

    if (IS_BAD_SEMAPHORE_ID(semaphoreId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_SEMAPHORE_ID;
    }

    p_semaphore = &semaphoreTable[semaphoreId];
    
    if (SEMAPHORE_FREE == p_semaphore->state)
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_SEMAPHORE_STATE;
    }

    p_semaphore->state = SEMAPHORE_FREE;

    reschedule_control(DEFER_START);

    while(p_semaphore->count < 0)
    {
        p_semaphore->count++;
        processId = q_get_first(p_semaphore->queueId);
        scheduler_set_ready_state(processId);
    }

    reschedule_control(DEFER_STOP);
    hal_restore_interrupts(intMask);
    return SW_OK;
}

SW sem_reset(SID32 semaphoreId, int32_t count)
{
    INTMASK intMask;
    Semaphore* p_semaphore;
    QID16 semQueueId;
    PID32 processId;

    intMask = hal_disable_interrupts();
    
    if (count < 0)
    {
        hal_restore_interrupts(intMask);
        return SW_NEGATIVE_SEM_COUNT_VALUE;
    }

    if (IS_BAD_SEMAPHORE_ID(semaphoreId))
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_SEMAPHORE_ID;
    }

    if (SEMAPHORE_FREE == semaphoreTable[semaphoreId].state)
    {
        hal_restore_interrupts(intMask);
        return SW_BAD_SEMAPHORE_STATE;
    }

    p_semaphore = &semaphoreTable[semaphoreId];
    semQueueId = p_semaphore->queueId;  // prepare to free any waiting processes on the queue

    reschedule_control(DEFER_START);

    while((processId = q_get_first(semQueueId)) != QTAB_EMPTY)
    {
        scheduler_set_ready_state(processId);
    }

    p_semaphore->count = count; // reset count as specified

    reschedule_control(DEFER_STOP);
    hal_restore_interrupts(intMask);
    return SW_OK;
}
