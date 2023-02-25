/**
 * @file   status_code.h
 * @brief  TICD framework response status codes
 */
#ifndef TICD_STANDARD_STATUS_CODE_H_INCLUDE
#define TICD_STANDARD_STATUS_CODE_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Header status field codes
 */
enum ticd_standard_status_code
{
    /**
     * @brief The application is running or task execution successful
     * 
     * This status code can take on different meanings depending on the running
     * task. For example, a boot status task response will populate the status
     * field with this value if the application is running. Another task that 
     * performs an action might respond with this value if the action was
     * successful.
     */
    TICD_STATUS_OK = 0,
    
    /**
     * @brief The system is (or is about to) reset
     * 
     * This status code is only really useful on systems that support
     * multicasting messages. An application with multicast capability might
     * broadcast a boot status response with this value in the status field
     * to let others know that this particular system is down. After booting
     * the application would then broadcast the relevant boot status to signal
     * that the application is no longer resetting.
     */
    TICD_STATUS_RESETTING,

    /**
     * @brief The received task is invalid
     * 
     * There are other finer grained status codes to describe most invalid task
     * states in the framework. This status code is here as a catch all for
     * those situations where a more descriptive status code is not available.
     */
    TICD_STATUS_TASK_INVALID,

    /**
     * @brief The application needs more initialization data before running
     * 
     * Some applications require initialization data before being in a
     * completely operational state. The framework provides a gating facility
     * to reject certain task until all initialization data has been received.
     * See @c ticd_glue_application_ready and @c ticd_glue_task_can_bypass_ready
     * for more information on the framework's task gating capability.
     */
    TICD_STATUS_NOT_READY,

    /**
     * @brief The received task's header field is invalid
     * 
     * If the received task's header field does not match the received bytes
     * return by the message receive callback, the task is rejected with a 
     * header length error. Applications can also reject a task with status
     * code if the header field does not match some pre-determined constant.
     */
    TICD_STATUS_HEADER_LEN_ERROR,

    /**
     * @brief The received task's header task ID is invalid
     * 
     * Whenever the application is tasked with a task ID that does not have a
     * registered handler, the framework will reject the task and response with
     * this status code. See @c ticd_register_task for more information about
     * register task handlers.
     */
    TICD_STATUS_HEADER_ID_ERROR,

    /**
     * @brief The received task's header sub-task ID is invalid
     */
    TICD_STATUS_HEADER_SUB_ID_ERROR,

    /**
     * @brief The received task's payload is invalid
     * 
     * Since the framework strives to be generic, payload errors are not
     * enumerated within the framework. Applications can define status codes
     * above (higher valued) the standard status codes if finer grained
     * reporting is required. It is the application developer's responsibility
     * to ensure that all involved parties understand the status codes and
     * their meanings.
     */
    TICD_STATUS_PAYLOAD_ERROR,

    /**
     * @brief The received task is valid but an error occurred during execution
     * 
     * Since the framework strives to be generic, execution errors are not
     * enumerated within the framework. Applications can define status codes
     * above (higher valued) the standard status codes if finer grained
     * reporting is required. It is the application developer's responsibility
     * to ensure that all involved parties understand the status codes and
     * their meanings.
     */
    TICD_STATUS_EXECUTION_ERROR,

    /**
     * @brief Status code 9 is reserved for future use
     */
    TICD_STATUS_RESERVED9,
    
    /**
     * @brief Status code 10 is reserved for future use
     */
    TICD_STATUS_RESERVED10,
    
    /**
     * @brief Status code 11 is reserved for future use
     */
    TICD_STATUS_RESERVED11,
    
    /**
     * @brief Status code 12 is reserved for future use
     */
    TICD_STATUS_RESERVED12,
    
    /**
     * @brief Status code 13 is reserved for future use
     */
    TICD_STATUS_RESERVED13,
    
    /**
     * @brief Status code 14 is reserved for future use
     */
    TICD_STATUS_RESERVED14,
    
    /**
     * @brief Status code 15 is reserved for future use
     */
    TICD_STATUS_RESERVED15,
};

#ifdef __cplusplus
}
#endif

#endif /* TICD_STANDARD_STATUS_CODE_H_INCLUDE */