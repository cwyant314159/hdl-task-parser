/**
 * @file   header.c
 * @brief  Implementation of TICD framework task header utilities
 * @author Cole Wyant
 * @date   2023-02-05
 */
#include "ticd/types/header.h"

/*
 * @brief 16-bit byte swap
 *
 * On big endian systems, the byte swap is a nop since the host and network
 * byte order is the same
 * 
 * @param[in] x 16-bit value to byte swap
 */
static inline uint32_t bswap16(uint16_t x)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return x;
#else
    return ((x & 0x00FF) << 8) |
           ((x & 0xFF00) >> 8) ;
#endif
}


/*
 * @brief 32-bit byte swap
 *
 * On big endian systems, the byte swap is a nop since the host and network
 * byte order is the same
 * 
 * @param[in] x 32-bit value to byte swap
 */
static inline uint32_t bswap32(uint32_t x)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return x;
#else
    return ((x & 0x000000FF) << 24) |
           ((x & 0x0000FF00) <<  8) |
           ((x & 0x00FF0000) >>  8) |
           ((x & 0xFF000000) >> 24) ;
#endif
}


void ticd_ntoh_header(TicdHeader * const p_hdr)
{
    // A byte swap is a byte swap. The framwework doesn't implement two different
    // header byte swaps. It just uses the other byte swapping function to do the
    // work.
    ticd_hton_header(p_hdr);
}


void ticd_hton_header(TicdHeader * const p_hdr)
{
    p_hdr->len_bytes    = bswap32(p_hdr->len_bytes);
    p_hdr->seq_id       = bswap16(p_hdr->seq_id);
    p_hdr->sub_task_id  = p_hdr->sub_task_id;
    p_hdr->task_id      = p_hdr->task_id;
    p_hdr->status       = bswap32(p_hdr->status);
}


void ticd_std_resp_header(const TicdHeader * const p_task_hdr, TicdHeader * const p_resp_hdr, uint32_t resp_status)
{
    p_resp_hdr->len_bytes   = TICD_HEADER_BYTES;
    p_resp_hdr->seq_id      = p_task_hdr->seq_id;
    p_resp_hdr->sub_task_id = p_task_hdr->sub_task_id;
    p_resp_hdr->task_id     = p_task_hdr->task_id;
    p_resp_hdr->status      = resp_status;
}