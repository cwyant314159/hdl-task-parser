/**
 * @file   ticd.h
 * @brief  The main TICD framework interface
 */
#ifndef TICD_H_INCLUDE
#define TICD_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include "ticd/types/error_codes.h"
#include "ticd/types/task_table_entry.h"

/**
 * @brief TICD framework initialize
 * 
 * Before the framework is able to process tasks and responses, it must be
 * initialized. The initialize method can be called multiple times (basically
 * resetting the framework). One of the side effects of initializing the
 * framework is that the internal lookup table is cleared. Task ID handlers
 * must be re-registered after initializing the framework.
 * 
 * Standard tasks are automatically registered by the framework. User
 * applications have the ability to overwrite entries in the look-up table as
 * needed.
 * 
 * @return TICD_SUCCESS if initialization succeeded. All other values indicate
 * initialization failed and task processing will likely not occur.
 */
TicdErr ticd_initialize(void);

/**
 * @brief Register task handlers with TICD framework
 * 
 * Tasks must be registered with the framework's task ID look-up table for
 * processing. Entries in this table have a mandatory handler that handles the
 * actual task execution as well as two optional handlers for byte-swapping the
 * payload fields of the task and response.
 * 
 * @param task_id TICD task ID
 * @param entry TICD framework look-up table entry
 * @return TICD_SUCCESS if registration succeeded. All other values indicate
 * registration failed and task processing will likely not occur.
 */
TicdErr ticd_register_task(uint8_t task_id, const TicdTaskTableEntry * const entry);

/**
 * @brief TICD framework main loop
 * 
 * This function handles the actual processing of the TICD framework. It must
 * be called regularly in the main loop. It must not be called before calling
 * @c ticd_initialize.
 */
void ticd_loop(void);

#ifdef __cplusplus
}
#endif

#endif /* TICD_H_INCLUDE */