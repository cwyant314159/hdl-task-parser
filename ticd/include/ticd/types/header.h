/**
 * @file   header.h
 * @brief  TICD framework task header structure
 */
#ifndef TICD_HEADER_H_INCLUDE
#define TICD_HEADER_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ticd/compiler.h"

/**
 * @brief TICD framework header structure.
 * 
 * The TICD header structure is a formatted data structure that is used by
 * the framework to determine which task handler is called and to communicate
 * the status of the task exection. This header structure is meant to be
 * transmitted over a physical medium and must not have any compiler added
 * padding between members.
 */
typedef struct PACKED
{
    /**
     * @brief Full task length in bytes
     * 
     * The length field is the size of the entire task in bytes. This size
     * includes header porition of the task. All valid lengths will fall 
     * within the range:
     * <br>
     * sizeof(TicdHeader) <= len_bytes <= sizeof(TicdHeader) + maximum payload size
     */
    uint32_t len_bytes;

    /**
     * @brief Task sequence ID
     * 
     * Sequence ID's are used to trace messages as they are passed around.
     * Controllers (i.e. the the system not running this framework) will
     * typically implement the sequence ID as a monotonically increasing
     * counter of messages. The framework itself does not use the sequence ID
     * in its task handling. It simply copies it from the inbound task's header
     * to the outbound reponses header.
     * 
     * There is no requirement that the sequence ID be a monotonically
     * increasing message counter. System developers are free to use this
     * field as needed. 
     */
    uint16_t seq_id;

    /**
     * @brief Sub-Task ID
     * 
     * Sub-task ID's are mainly intended for application developers to give
     * finer grain control over task execution. An example of where a sub-task
     * ID might be useful is when applications require multiple types of
     * initialization data. The @c TICD_TASK_INITIALIZATION_DATA task can be
     * used to bypass the ready state gate and sub-task IDs can be used to
     * determine which type of initialization data is used within the payload
     * portion of the task. 
     * 
     * The framework does not use the sub-task ID except to verify that the
     * standard tasks have sub-task ID's of zero. Application specific tasks
     * are free to use this field as needed.
     */
    uint8_t  sub_task_id;

    /**
     * @brief Task ID
     * 
     * Task ID's are used by the framework to determine which handler from the
     * task look-up table to call. The framework uses this field extensively,
     * and it cannot be used for anything other than its intended purpose.
     */
    uint8_t  task_id;

    /**
     * @brief Task status code
     * 
     * The status field is the frameworks main way of indicating errors and
     * reporting status. When processing malformed tasks or handling standard
     * tasks, the framework will report success or the failure mode to the 
     * controlling application using the status field. Application specific
     * tasks are not required to use the status field in this way, but it is
     * heavily recommended.
     * 
     * An inbound task's status field is not used by the framework and does not
     * usually have any meaning in task execution. It is merely provided to
     * reduce the number of unique types in the framework and provide a more
     * consistent messaging protocol.
     */
    uint32_t status;
} TicdHeader;

/**
 * @brief The number of bytes in a TICD header
 */
#define TICD_HEADER_BYTES (sizeof(TicdHeader))

/**
 * @brief The number of 32-bit words in a TICD header
 */
#define TICD_HEADER_WORDS (TICD_HEADER_BYTES / 4)

/**
 * @brief TICD framework header network to host byte swap
 * 
 * The framework assumes that received messages have been byte swapped using
 * the same HTON/NTOH scheme found in most TCP/IP applications. This function
 * will do the necessary work to byte swap a task header from network byte
 * order to host byte order. Application writers typically do not need to use
 * this function. The framework will handle byte swapping before passing tasks
 * to handlers.
 * 
 * This function is capable of determining byte ordering.
 * 
 * @param[in,out] p_hdr Pointer to a header structure
 */
void ticd_ntoh_header(TicdHeader * const p_hdr);

/**
 * @brief TICD framework header host to network byte swap
 * 
 * The framework assumes that transmitted messages must be byte swapped using
 * the same HTON/NTOH scheme found in most TCP/IP applications. This function
 * will do the necessary work to byte swap a task header from host byte order
 * to network byte order. Application writers typically do not need to use this
 * function. The framework will handle byte swapping before passing the task to
 * the transmit function.
 * 
 * This function is capable of determining byte ordering.
 * 
 * @param[in,out] p_hdr Pointer to a header structure
 */
void ticd_hton_header(TicdHeader * const p_hdr);

/**
 * @brief TICD framework standard response header population
 * 
 * Most responses in a TICD application will be a standard response (header
 * only). A standard response header is bascially a copy of the task's header
 * with the status code set to whatever is appropriate from the task execution.
 * 
 * @param[in] p_task_hdr Pointer to a header structure of the inbound task
 * @param[out] p_resp_hdr Pointer to a header structure of the outbound response
 * @param[in] resp_status New response header status field
 */
void ticd_std_resp_header(const TicdHeader * const p_task_hdr, TicdHeader * const p_resp_hdr, uint32_t resp_status);

#ifdef __cplusplus
}
#endif

#endif /* TICD_HEADER_H_INCLUDE */