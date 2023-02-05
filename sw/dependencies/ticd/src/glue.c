/**
 * @file   glue.c
 * @brief  TICD framework weakly defined "glue" function defaults
 * @author Cole Wyant
 * @date   2023-02-04
 */
#include "ticd/glue.h"

/*
 * @brief Default reset action
 *
 * In the default case, an application does not have a reset action, and this
 * function is basically a NOP.
 */
void __attribute__((weak)) ticd_glue_reset_action(void)
{
    return;
}

/*
 * @brief Default application ready check
 *
 * In the default case, an application does not have any required
 * initialization data, and this function simply returns a success.
 * 
 * @return @c TICD_SUCCESS
 */
TicdErr __attribute__((weak)) ticd_glue_application_ready(void)
{
    return TICD_SUCCESS;
}

/*
 * @brief Default application task ID bypass check
 *
 * In the default case, none of the application's task ID's can bypass the
 * initialization data check, and this function simply returns a failure.
 * 
 * @return @c TICD_FAILURE
 */
TicdErr __attribute__((weak)) ticd_glue_task_can_bypass_ready(uint8_t task_id)
{
    return TICD_FAILURE;
}