/**
 * @file   message.c
 * @brief  TICD framework message structure helper implementations
 * @author Cole Wyant
 * @date   2023-02-05
 */
#include "ticd/types/message.h"

#include <string.h>


TicdErr ticd_message_populate_source(TicdMsg * const msg, const uint8_t * const meta_buf, size_t meta_buf_len)
{
    if (meta_buf_len > TICD_META_BUF_LEN) {
        return TICD_FAILURE;
    }

    memcpy(msg->meta, meta_buf, meta_buf_len);
    
    return TICD_SUCCESS;
}


TicdErr ticd_message_populate_task(TicdMsg * const msg, const uint8_t * const task_buf, size_t task_buf_len)
{
    if (task_buf_len > sizeof(TicdTask)) {
        return TICD_FAILURE;
    }

    memcpy(&msg->task, task_buf, task_buf_len);

    return TICD_SUCCESS;
}