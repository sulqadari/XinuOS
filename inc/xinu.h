#ifndef _H_XINU
#define _H_XINU

//#include <string.h>
//#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../hal/hal_common.h"
#include "globals.h"
#include "process.h"    //! process.h file must precede queue.h file 
#include "queue.h"
#include "status.h"
#include "scheduler.h"
#include "systemcall.h"
#include "message.h"
#include "device.h"
#include "ll_memory.h"
#include "semaphore.h"

#endif // !_H_XINU