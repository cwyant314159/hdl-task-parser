/**
 * @file   task_table_entry.h
 * @brief  TICD framework task look-up table entry structure
 */
#ifndef TICD_TASK_TABLE_ENTRY_H_INCLUDE
#define TICD_TASK_TABLE_ENTRY_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ticd/types/task.h"

/**
 * @brief Task payload byte-swap (hton) callback
 * 
 * A typical task payload byte-swap will typecast the p_payload pointer to an
 * application specific payload pointer and do a member by member byte-swap.
 * 
 * @param[in,out] p_payload Pointer to the task's payload
 */
typedef void (*TicdTaskPayloadHton)(uint8_t * const p_payload);

/**
 * @brief Response payload byte-swap (ntoh) callback
 * 
 * A typical respons payload byte-swap will typecast the p_payload pointer to
 * an application specific payload pointer and do a member by member byte-swap.
 * 
 * @param[in,out] p_payload Pointer to the response's payload
 */
typedef void (*TicdRespPayloadNtoh)(uint8_t * const p_payload);

/**
 * @brief Task handler callback
 * 
 * By the time the task handler is called, the inbound task header and payload
 * have been byte-swapped. The task parameter should not need to be mutated in
 * the handler.
 * 
 * @param[in] task Pointer to the inbound task @c TicdTask struct
 * @param[out] resp Pointer to the outbound response @c TicdTask struct
 */
typedef void (*TicdTaskHandler)(const TicdTask * const task, TicdTask * const resp);

/**
 * @brief TICD internal task table entry
 * 
 * The TICD framework keeps a table of handlers that is indexed by the task ID.
 * This structure bundles all the callbacks of a particular task ID into an
 * easy to use structure. If a given callback is not required (i.e. a header
 * only task with no payload), the callback's value can be set to NULL.
 */
typedef struct ticd_task_table_entry
{
    /**
     * @brief The entry's task handler
     * 
     * This is the only required member of this struct. If this value is set to
     * NULL, the framework treats the task ID associated with this entry as not
     * implemented and will reject the task.
     */
    TicdTaskHandler     handler;
    
    /**
     * @brief The entry's task payload HTON
     * 
     * This member is not required. If the associated task ID is a header only
     * task, this member can be set to NULL.
     */
    TicdTaskPayloadHton task_hton;
    
    /**
     * @brief The entry's response payload NTOH
     * 
     * This member is not required. If the associated task ID's response is 
     * header only, this member can be set to NULL.
     */
    TicdRespPayloadNtoh resp_ntoh;
} TicdTaskTableEntry;

#ifdef __cplusplus
}
#endif

#endif /* TICD_STANDARD_TASK_CODES_H_INCLUDE */