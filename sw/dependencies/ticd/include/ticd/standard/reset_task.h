/**
 * @file   reset_task.h
 * @brief  Standard reset task TICD framework callbacks
 * @author Cole Wyant
 * @date   2023-02-04
 */
#ifndef TICD_STANDARD_RESET_TASK_H_INCLUDE
#define TICD_STANDARD_RESET_TASK_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "ticd/types/task.h"

/**
 * @brief Standard reset task handler
 * 
 * The standard reset task is used by external applications to reset the
 * system. Typically when this task is executed the control tasking framework
 * would broadcast to the network that the system is about to reset with an
 * unsolicited Boot Status task. However, this framework is a toy example and
 * does not have multicasting capability.
 * 
 * This task handler simply populates the response and calls the reset action
 * glue callback. It is up to the application writer to determine if the reset
 * action returns, so that the response message can be transmitted.
 * 
 * @param[in] task Inbound task
 * @param[out] resp Outbound task response
 */
void ticd_standard_reset_task_handler(const TicdTask * const task, TicdTask * const resp);

#ifdef __cplusplus
}
#endif

#endif /* TICD_STANDARD_TASK_ID_H_INCLUDE */