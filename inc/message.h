#ifndef _H_MESSAGE
#define _H_MESSAGE

#include <stdint.h>
#include "globals.h"

/**
 * @brief  TODO: requires implementation
 * @note   
 * @param  parentPid: 
 * @param  processId: 
 * @retval 
 */
SW syscall_send(PID32 parentPid, PID32 processId);

#endif  // !_H_MESSAGE