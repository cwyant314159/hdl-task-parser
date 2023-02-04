/**
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// TODO doxygen

typedef struct ticd_header 
{
    uint32_t len_bytes;
    uint16_t seq_id;
    uint8_t  reserved0;
    uint8_t  task_id;
    uint32_t status;
} ticd_header_t __attribute__((packed));

#define TICD_HEADER_BYTES (sizeof(ticd_header_t))
#define TICD_HEADER_WORDS (TICD_HEADER_BYTES / 4)

void ticd_ntoh_header(const ticd_header_t * const p_net,  ticd_header_t * const p_host);
void ticd_hton_header(const ticd_header_t * const p_host, ticd_header_t * const p_net);
void ticd_std_resp_header(const ticd_header * const p_task_hdr, ticd_header_t * const p_resp_hdr);

#ifdef __cplusplus
}
#endif
