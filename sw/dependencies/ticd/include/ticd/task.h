/**
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ticd/header.h"

#ifndef TICD_MAX_MESSAGE_PAYLOAD_BYTES
#define TICD_MAX_MESSAGE_PAYLOAD_BYTES (32U)
#endif

// TODO doxygen
typedef struct ticd_task
{
    ticd_header_t header;
    uint8_t       payload[TICD_MAX_MESSAGE_PAYLOAD_BYTES];
} ticd_task_t __attribute__((packed));

#ifdef __cplusplus
}
#endif
