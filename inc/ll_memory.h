#ifndef _H_MEMORY
#define _H_MEMORY

#include <stdint.h>
#include "globals.h"

/**
 * @brief  TODO: requires implementation
 * @note   
 * @param  stackBase: 
 * @param  stackLen: 
 * @retval None
 */
void mem_free_stack(uint8_t* stackBase, uint32_t stackLen);

/**
 * @brief  TODO: requires implementation
 * @note   
 * @param  stackSize: 
 * @retval 
 */
uint32_t mem_round_mb(uint32_t stackSize);

/**
 * @brief  TODO: requires implementation
 * @note   
 * @param  stackSize: 
 * @retval 
 */
STACK mem_alloc_stack(uint32_t stackSize);
#endif  // !_H_MEMORY