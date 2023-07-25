/*
 * @file   reset_task.c
 * @brief  Implementation of the standard reset task handler
 * @author Cole Wyant
 * @date   2023-02-04
 */
#include "ticd/standard/reset_task.h"

#include "ticd/glue.h"
#include "ticd/standard/status_code.h"

/*
 * @brief Boot status task sub-task ID
 */
#define RESET_SUBTASK_ID    0U

/*
 * @brief Valid reset task length in bytes
 */
#define RESET_TASK_LEN      sizeof(TicdHeader)

/*
 * @brief Standard reset task's header validation helper
 * 
 * @param[in] hdr Task header
 * 
 * @return TICD_STATUS_OK if all fields are valid; otherwise a status code
 * indicating the invalid field.
 */
static uint32_t validate_header(const TicdHeader * const hdr)
{
    if (hdr->len_bytes != RESET_TASK_LEN) {
        return TICD_STATUS_HEADER_LEN_ERROR;
    } else if (hdr->sub_task_id != RESET_SUBTASK_ID) { 
        return TICD_STATUS_HEADER_SUB_ID_ERROR;
    } else {
        return TICD_STATUS_OK;
    }
}


void ticd_standard_reset_task_handler(const TicdTask * const p_task, TicdTask * const p_resp)
{
    // A reset command has no payload. The header validation is the reponse
    // message status.
    const uint32_t status = validate_header(&p_task->header);
    ticd_std_resp_header(&p_task->header, &p_resp->header, status);

    // Typically at this point the framework would prepare to handle a system
    // reset. This usually means broadcasting to the network that the system is
    // about to reset with an unsolicited Boot Status task. For this example
    // framework, there is no broadcasting ability, so there is nothing to do.

    // Reset the system using the user provided glue function. In a real design
    // this function would not return.
    ticd_glue_reset_action();
}