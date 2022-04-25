#ifndef _H_HAL_STM32F103C8T6
#define _H_HAL_STM32F103C8T6

#include <stdint.h>

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

#endif // !_H_HAL_STM32F103C8T6