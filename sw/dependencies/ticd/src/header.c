/**
 */

#include "ticd/header.h"

static inline uint32_t bswap16(uint16_t x)
{
    return ((x & 0x00FF) << 8) |
           ((x & 0xFF00) >> 8) ;
}

static inline uint32_t bswap32(uint32_t x)
{
    return ((x & 0x000000FF) << 24) |
           ((x & 0x0000FF00) <<  8) |
           ((x & 0x00FF0000) >>  8) |
           ((x & 0xFF000000) >> 24) ;
}

void ticd_ntoh_header(const ticd_header_t * const p_net,  ticd_header_t * const p_host)
{
    ticd_hton_header(p_net, p_host);
}

void ticd_hton_header(const ticd_header_t * const p_host, ticd_header_t * const p_net)
{
    p_net->len_bytes = bswap32(p_host->len_bytes);
    p_net->seq_id    = bswap16(p_host->seq_id);
    p_net->reserved0 = p_host->reserved0;
    p_net->task_id   = p_host->task_id;
    p_net->status    = bswap32(p_host->status);
}

void ticd_std_resp_header(const ticd_header * const p_task_hdr, ticd_header_t * const p_resp_hdr)
{
    *p_resp_hdr = *p_task_hdr;
}

