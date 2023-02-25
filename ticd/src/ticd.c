/*
 * @file   ticd.c
 * @brief  Implementation of TICD framework
 * @author Cole Wyant
 * @date   2023-02-05
 */
#include "ticd/ticd.h"

#include <string.h>
#include "ticd/glue.h"
#include "ticd/types/message.h"
#include "ticd/standard/reset_task.h"
#include "ticd/standard/status_code.h"
#include "ticd/standard/task_id.h"

// task table
#ifndef TICD_LOOKUP_TABLE_SIZE
    
    /*
     * @brief The size of the task ID handler look-up table in task ID's
     */
    #define TICD_LOOKUP_TABLE_SIZE 256U

#endif

/*
 * @brief Task ID handler look-up table
 */
static TicdTaskTableEntry task_table[TICD_LOOKUP_TABLE_SIZE];

/*
 * @brief Memory for inbound task messages
 */
static TicdMsg task_msg;

/*
 * @brief Memory for outbound response messages
 */
static TicdMsg resp_msg;

/*
 * @brief Task struct pointer to the inboud task message's task byte array
 */
static TicdTask * const p_task = task_msg.task;

/*
 * @brief Task struct pointer to the outboud response message's task byte array
 */
static TicdTask * const p_resp = resp_msg.task;

/*
 * @brief Execute the current task
 * 
 * Using the static global pointers, look-up the task in the task table and
 * execute all the handlers in the task's entry.
 */
static void execute_task(void)
{
    // Retrieve the task ID's entry from the table using the static global task
    // pointer and call all necessary functions.
    const TicdTaskTableEntry * const entry = &task_table[p_task->header.task_id];

    // If a task payload HTON function was provided at task registration, call
    // it now with the static global task payload pointer.
    if (entry->task_hton != NULL) {
        entry->task_hton(p_task->payload);
    }

    // The handle has already been checked for NULL in the task ID
    // pre-conditions. Call the handle with the static global task and response
    // pointer.
    entry->handler(p_task, p_resp); // uses static global objects

    // If a response payload NTOH function was provided at task registration,
    // call it now with the static global response payload pointer.
    if (entry->resp_ntoh != NULL) {
        entry->resp_ntoh(p_resp->payload);
    }
}

/*
 * @brief Reject the current task with a status code
 * 
 * Using the static global pointers, populate the response with a task reject
 * with the status field set to the given value.
 * 
 * @param[in] status_code Response's status code
 */
static void reject_task(uint32_t status_code)
{
    ticd_std_resp_header(&p_task->header, &p_resp->header, status_code);
}

/*
 * @brief Check to see if the task ID has a valid handler
 * @param[in] id Task ID
 * @return TICD_SUCCESS if the given ID has a handler. All other values
 * indicate there is no valid handler.
 */
static TicdErr task_has_handler(uint8_t id)
{
    if (id < TICD_LOOKUP_TABLE_SIZE && task_table[id].handler != NULL) {
        return TICD_SUCCESS;
    } else {
        return TICD_FAILURE;
    }
}

/*
 * @brief Check to see if the task ID can bypass the application ready gate
 * @param[in] id Task ID
 * @return TICD_SUCCESS if the given ID does not need to wait for the
 * application to be ready. All other values indicate the task ID must be
 * rejected when the application is not ready.
 */
static TicdErr task_can_bypass_ready(uint8_t id)
{
    return (TICD_TASK_BOOT_STATUS == id)         ? TICD_SUCCESS :
           (TICD_TASK_RESET       == id)         ? TICD_SUCCESS :
           (TICD_TASK_INITIALIZATION_DATA == id) ? TICD_SUCCESS :
                                                   ticd_glue_task_can_bypass_ready(id);
}

TicdErr ticd_initialize(void)
{
    // Zero out the task and response message structures.
    memset(&task_msg, 0x00, sizeof(task_msg));
    memset(&resp_msg, 0x00, sizeof(resp_msg));

    // Clear the table with 0's so that NULL checks in the processing loop work
    // as expected.
    memset(task_table, 0x00, sizeof(task_table));
    
    // Populate the standard commands that must be present for all
    // applications.
    const TicdTaskTableEntry reset_task_entry = {
        .handler   = ticd_standard_reset_task_handler,
        .task_hton = NULL,
        .resp_ntoh = NULL
    };
    const TicdErr reset_rv = ticd_register_task(TICD_TASK_RESET, &reset_task_entry);

    const TicdTaskTableEntry boot_status_task_entry = {
        .handler   = NULL,
        .task_hton = NULL,
        .resp_ntoh = NULL
    };
    const TicdErr boot_rv  = ticd_register_task(TICD_TASK_BOOT_STATUS, &boot_status_task_entry);
    
    if (reset_rv != TICD_SUCCESS || boot_rv != TICD_SUCCESS) {
        return TICD_FAILURE;
    }

    return TICD_SUCCESS;
}

TicdErr ticd_register_task(uint8_t task_id, const TicdTaskTableEntry * const entry)
{
    if (task_id < TICD_LOOKUP_TABLE_SIZE) {
        task_table[task_id] = *entry;
        return TICD_SUCCESS;
    }

    return TICD_FAILURE;
}

void ticd_loop(void)
{

    // Attempt to receiver a message from the user provided glue function.
    // Processing only occurs if at least 1 bytes is received. A value of 0
    // indicates that no message was available for processing and causes the
    // loop to exit early. Values less than 0 indicate an error condition on
    // the receiver interface. This condition is also ignored by the processing
    // loop.
    const int bytes_received = ticd_glue_rx_message(&task_msg);
    if (bytes_received < 1) {
        return;
    }
    
    // At least 1 bytes has been received. Copy the meta buffer from the task
    // message structure to the response message structure. The user app will
    // use this buffer to determine where to send the response message.
    memcpy(resp_msg.meta, task_msg.meta, TICD_META_BUF_LEN);

    // Byte swap the header even though we might not have a full headers worth
    // of data. Since we have allocated enough space for the largest possible
    // message, it's safe to do so. See the definition of the TicdMsg struct.
    ticd_ntoh_header(p_task);

    // The following if-else if-else structure is the main decision tree of the
    // TICD library.
    if(bytes_received != p_task->header.len_bytes) {

        // The bytes received by the user provided glue function does not match
        // the length bytes in the message header. There is no way to know who
        // is right.
        reject_task(TICD_STATUS_HEADER_LEN_ERROR);

    } else if(TICD_SUCCESS != task_has_handler(p_task->header.task_id)) {
        
        // There is no handler for the given task ID.
        reject_task(TICD_STATUS_HEADER_ID_ERROR);
    
    } else if(TICD_SUCCESS == task_can_bypass_ready(p_task->header.task_id)) {
        
        // The header length is coherent and there is a valid handler in the
        // task table, but this task ID is not gated by the application
        // ready state.
        execute_task();
    }
    
    else if (TICD_SUCCESS != ticd_glue_application_ready()) {

        // The application is not in a ready state and the task ID is not
        // allowed to execute when the application is not ready.
        reject_task(TICD_STATUS_NEED_INITIALIZATION_DATA);

    } else {

        // The header length is coherent, there is a valid handle in the task
        // table, and the application is in a ready state.
        execute_task();
    }

    // Transmit the response message using the user provided glue function. All
    // transmission error handling is left to the user application. TICD does
    // not care about errors or status from the transmission medium.
    ticd_glue_tx_message(&resp_msg);
}

/**
 * @brief Returns success if the task has a valid handler in the task table.
 */
static TicdErr task_has_handler(uint8_t id)
{
    if (id < TICD_LOOKUP_TABLE_SIZE && task_table[id].handler != NULL) {
        return TICD_SUCCESS;
    } else {
        return TICD_FAILURE;
    }
}


// Compile time checks
#if (TICD_LOOKUP_TABLE_SIZE-1) > UINT8_MAX
    #error TICD_LOOKUP_TABLE_SIZE too big for task ID integer width
#endif