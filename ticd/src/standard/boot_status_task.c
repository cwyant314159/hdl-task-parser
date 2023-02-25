/*
 * @file   boot_status_task.c
 * @brief  Implementation of the standard boot status task handler
 * @author Cole Wyant
 * @date   2023-02-04
 */
#include "ticd/standard/boot_status_task.h"

#include "ticd/glue.h"
#include "ticd/standard/status_code.h"

/*
 * @brief Boot status task sub-task ID
 */
#define BOOT_STATUS_SUBTASK_ID  0U

/*
 * @brief Valid boot status task length in bytes
 */
#define BOOT_STATUS_TASK_LEN    sizeof(TicdHeader) // header only task

/*
 * @brief Standard boot status task's header validation helper
 *
 * @param[in] hdr Task header
 *
 * @return TICD_STATUS_OK if all fields are valid; otherwise a status code
 * indicating the invalid field.
 */
static uint32_t validate_header(const TicdHeader * const hdr)
{
    if (hdr->len_bytes != BOOT_STATUS_TASK_LEN) {
        return TICD_STATUS_HEADER_LEN_ERROR;
    } else if (hdr->sub_task_id != BOOT_STATUS_SUBTASK_ID) {
        return TICD_STATUS_HEADER_SUB_ID_ERROR;
    } else {
        return TICD_STATUS_OK;
    }
}

/*
 * @brief Definition of the standard boot status task handler
 */
void ticd_standard_boot_status_task_handler(const TicdTask * const p_task, TicdTask * const p_resp)
{
    // If the header of the boot status task is not valid, the response
    // status is just the return value of the header validation helper. If the
    // task header is valid, the response status is determined by the
    // application initialization state. It is either not ready or in an OK
    // status state.
    const uint32_t validate_status = validate_header(&p_task->header);

    const uint32_t status = (validate_status != TICD_STATUS_OK)             ? validate_status       :
                            (TICD_SUCCESS == ticd_glue_application_ready()) ? TICD_STATUS_OK        :
                                                                              TICD_STATUS_NOT_READY ;

    ticd_std_resp_header(&p_task->header, &p_resp->header, status);
}