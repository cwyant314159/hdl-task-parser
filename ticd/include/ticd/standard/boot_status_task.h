/**
 * @file   boot_status_task.h
 * @brief  Standard boot status task TICD framework callbacks
 */
#ifndef TICD_STANDARD_BOOT_STATUS_TASK_H_INCLUDE
#define TICD_STANDARD_BOOT_STATUS_TASK_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "ticd/types/task.h"


/**
 * @brief Standard boot status task handler
 * 
 * The standard boot status task is used by external applications to probe the
 * system's running state. Assuming that the task is valid (i.e. the header is
 * valid), this task can response with one of two status codes:
 * 
 * TICD_STATUS_OK - The application has received all initialization data (or
 * does not need it) and is in the running state.
 * 
 * TICD_STATUS_NOT_READY - The application is not in a ready state and some 
 * commands might be gated.
 * 
 * @param[in] task Inbound task
 * @param[out] resp Outbound task response
 */
void ticd_standard_boot_status_task_handler(const TicdTask * const task, TicdTask * const resp);


#ifdef __cplusplus
}
#endif

#endif /* TICD_STANDARD_TASK_ID_H_INCLUDE */