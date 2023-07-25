/**
 * @file   error_codes.h
 * @brief  TICD framework software error codes
 */
#ifndef TICD_ERROR_CODES_H_INCLUDE
#define TICD_ERROR_CODES_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief TICD framework software error codes
 */
typedef enum ticd_software_error_codes
{
    /**
     * @brief Operation or check succeeded/valid/true/good/accept.
     */
    TICD_SUCCESS = 0,

    /**
     * @brief Operation or check failed/invalid/false/bad/reject.
     */
    TICD_FAILURE = 1
} TicdErr;

#ifdef __cplusplus
}
#endif

#endif /* TICD_HEADER_H_INCLUDE */