/**
 * @file   task_id.h
 * @brief  TICD framework task ID codes
 * @author Cole Wyant
 * @date   2023-02-04
 */
#ifndef TICD_STANDARD_TASK_ID_H_INCLUDE
#define TICD_STANDARD_TASK_ID_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Header task ID field identifiers
 */
enum ticd_standard_task_id
{
    /**
     * @brief Task ID 0 is reserved for future use
     */
    TICD_TASK_RESERVED0 = 0,

    /**
     * @brief System/application boot status
     * 
     * Upon a reset, an application can be in either a running state or an idle
     * state waiting for initialization data. This task allows external systems
     * to probe the application for its current state. This task is not gated
     * by the additional initialization data required state.
     */
    TICD_TASK_BOOT_STATUS,
    
    /**
     * @brief System reset
     * 
     * To remain generic, the actual reset functionality is implemented in the
     * user supplied glue function @c ticd_glue_reset_action. The TICD
     * framework is only responsible for task validation and response
     * generation for this task. This task is not gated by the additional
     * initialization data required state.
     */
    TICD_TASK_RESET,
    
    /**
     * @brief Application initialization data
     * 
     * Some applications may require additional data from an external source
     * before processing tasks. This task is provided to allow the additional
     * data to reach the application. This task is not gated by the additional
     * initialization data required state.
     */
    TICD_TASK_INITIALIZATION_DATA,
    
    /**
     * @brief Task ID 4 is reserved for future use
     */
    TICD_TASK_RESERVED4,
    
    /**
     * @brief Task ID 5 is reserved for future use
     */
    TICD_TASK_RESERVED5,
    
    /**
     * @brief Task ID 6 is reserved for future use
     */
    TICD_TASK_RESERVED6,
    
    /**
     * @brief Task ID 7 is reserved for future use
     */
    TICD_TASK_RESERVED7,
    
    /**
     * @brief Task ID 8 is reserved for future use
     */
    TICD_TASK_RESERVED8,
    
    /**
     * @brief Task ID 9 is reserved for future use
     */
    TICD_TASK_RESERVED9,
    
    /**
     * @brief Task ID 10 is reserved for future use
     */
    TICD_TASK_RESERVED10,
    
    /**
     * @brief Task ID 11 is reserved for future use
     */
    TICD_TASK_RESERVED11,
    
    /**
     * @brief Task ID 12 is reserved for future use
     */
    TICD_TASK_RESERVED12,
    
    /**
     * @brief Task ID 13 is reserved for future use
     */
    TICD_TASK_RESERVED13,
    
    /**
     * @brief Task ID 14 is reserved for future use
     */
    TICD_TASK_RESERVED14,

    /**
     * @brief Task ID 15 is reserved for future use
     */
    TICD_TASK_RESERVED15
};

#ifdef __cplusplus
}
#endif

#endif /* TICD_STANDARD_TASK_ID_H_INCLUDE */