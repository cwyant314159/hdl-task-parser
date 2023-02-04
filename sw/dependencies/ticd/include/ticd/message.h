/**
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ticd/task.h"

#define TICD_SOURCE_STRING_LEN (80U)

// TODO doxygen
typedef struct ticd_message
{
    char        source[TICD_SOURCE_STRING_LEN];
    ticd_task_t task;
} ticd_msg __attribute__((packed));

#ifdef __cplusplus
}
#endif
