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
 * @brief  Creates stack image
 * @note   
 * @retval None
 */
void hal_create_stack_image(void);

#endif // !_H_HAL_STM32F103C8T6