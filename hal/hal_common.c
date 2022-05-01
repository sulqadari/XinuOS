#include "hal_common.h"


void hal_switch_context(uint8_t* currProcStkPtr, uint8_t* nextProcStkPtr)
{

}

void hal_halt(void)
{
    
}

INTMASK hal_disable_interrupts(void)
{
    return 0;
}

void hal_restore_interrupts(INTMASK intMask)
{

}

void hal_free_stack(uint8_t* stackBase, uint32_t stackLen)
{

}

STACK hal_alloc_stack(uint32_t stackSize)
{
    return STACK;
}

void hal_create_stack_image(void)
{
    
}