/**
 * @file   task.h
 * @brief  TICD framework task structure
 * @author Cole Wyant
 * @date   2023-02-05
 */
#ifndef TICD_TASK_H_INCLUDE
#define TICD_TASK_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ticd/types/header.h"

#ifndef TICD_MAX_MESSAGE_PAYLOAD_BYTES

    /**
     * @brief Maximum possible task payload size in bytes
     */
    #define TICD_MAX_MESSAGE_PAYLOAD_BYTES (32U)

#endif

/**
 * @brief TICD framework task/response structure
 * 
 * The task structure is the main unit of work in the TICD framework. The 
 * structure consit of a header structure and payload array. The header
 * structure is used by the framework to coordinate task and response handling.
 * Payload arrays are memory areas where task handlers can pull action
 * information or dump response data. In a typical application handler, a
 * specific payload structure pointer will point to the payload field to access
 * the payload information.
 * 
 * This task structure is meant to be transmitted over a physical medium and
 * must not have any compiler added padding between members. The structure must
 * be "packed".
 */
typedef struct ticd_task
{
    /**
     * @brief TICD header structure
     */
    TicdHeader header;
    
    /**
     * @brief Payload byte array
     */
    uint8_t    payload[TICD_MAX_MESSAGE_PAYLOAD_BYTES];
} TicdTask __attribute__((packed));

#ifdef __cplusplus
}
#endif

#endif /* TICD_TASK_H_INCLUDE */
