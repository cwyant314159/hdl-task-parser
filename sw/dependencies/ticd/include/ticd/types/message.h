/**
 * @file   message.h
 * @brief  TICD framework message structure
 * @author Cole Wyant
 * @date   2023-02-05
 */
#ifndef TICD_MESSAGE_H_INCLUDE
#define TICD_MESSAGE_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "ticd/types/error_codes.h"
#include "ticd/types/task.h"

#ifndef TICD_META_BUF_LEN

    /**
     * @brief Message structure meta buffer size in bytes
     */
    #define TICD_META_BUF_LEN (80U)

#endif

/**
 * @brief TICD framework message structure
 * 
 * The message structure is a meta structure in that it contains a byte buffer
 * for the task (or reponse) and a buffer for meta information used by the
 * transmit and receiver infrastructure.
 * 
 * A message structure contains a byte buffer for the task (header and payload)
 * and a byte buffer for messaging meta information. The meta buffer is copied
 * from the inbound task message to the outbound reponse message inside the
 * framework. The framework does not modify or use the meta buffer during task
 * processing. The intent of the meta buffer is to allow the message reception
 * logic to pass information to the transmission logic (i.e. the sender of the
 * task or the address of the response recipient).
 * 
 * This message structure is meant to be transmitted over a physical medium and
 * must not have any compiler added padding between members. The structure must
 * be "packed".
 */
typedef struct ticd_message
{
    /**
     * @brief Message meta information byte array
     */
    uint8_t meta[TICD_META_BUF_LEN];

    /**
     * @brief TICD task structure byte array
     */
    uint8_t task[sizeof(TicdTask)];
} TicdMsg __attribute__((packed));

/**
 * @brief Message meta buffer helper
 * 
 * @param[in,out] msg Pointer to TICD message structure
 * @param[in] meta_buf Pointer to the meta information for the message
 * @param[in] meta_buf_len Length of the meta information in bytes
 * @return TICD_SUCCESS if the populate was successful; all other values
 * indicate failure
 */
TicdErr ticd_message_populate_source(TicdMsg * const msg, const uint8_t * const meta_buf, size_t meta_buf_len);

/**
 * @brief Message task buffer helper
 * 
 * @param[in,out] msg Pointer to TICD message structure
 * @param[in] task_buf Pointer to the task data
 * @param[in] task_buf_len Length of the task data in bytes
 * @return TICD_SUCCESS if the populate was successful; all other values
 * indicate failure
 */
TicdErr ticd_message_populate_task(TicdMsg * const msg, const uint8_t * const task_buf, size_t task_buf_len);

#ifdef __cplusplus
}
#endif

#endif /* TICD_MESSAGE_H_INCLUDE */
