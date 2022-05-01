#ifndef _H_HAL_STM32F103C8T6
#define _H_HAL_STM32F103C8T6

#include <stdint.h>
#include "../inc/globals.h"

/**
 * @brief  terminates processor
 * @note   
 * @retval None
 */
void hal_halt(void);

/**
 * @brief Stores context of current process into its stack:
 * 
 * 1. push the contents of the processor registers on the stack;
 * 2. save the currStackPtr in Process.stackPointer;
 * 3. load the nextStackPtr of the 'new' process;
 * 4. reload the processor registers from values previously saved on the stack of 'new' process;
 * 5. return to the function in the new process that called this funtion earlier.
 * 
 * @note   
 * @param  uint8_t* currStackPtr: stack pointer of the process which called this function
 * @param  uint8_t* nextStackPtr: stack pointer of new process
 * @retval None
 */
void hal_switch_context(uint8_t* currStackPtr, uint8_t* nextStackPtr);

/**
 * @brief  Disables interruptions for the sake of preventing a context switching and consequent
 * changing global data objects by another process.
 * @note   
 * @retval 
 */
INTMASK hal_disable_interrupts(void);

/**
 * @brief  Enables interruptions.
 * @note   
 * @param  intMask: 
 * @retval None
 */
void hal_restore_interrupts(INTMASK intMask);

/**
 * @brief  Frees memory that has been allocated for the process's stack
 * @note   
 * @param  stackBase: 
 * @param  stackLen: 
 * @retval None
 */
void hal_free_stack(uint8_t* stackBase, uint32_t stackLen);

/**
 * @brief  Allocates memory in stack area for the new process
 * @note   
 * @param  stackSize: 
 * @retval 
 */
STACK hal_alloc_stack(uint32_t stackSize);

/**
 * @brief  Creates stack image
 * @note   
 * @retval None
 */
void hal_create_stack_image(void);

#endif // !_H_HAL_STM32F103C8T6