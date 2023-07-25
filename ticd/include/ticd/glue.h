/**
 * @file   glue.h
 * @brief  TICD framework user provided "glue" functions
 */
#ifndef TICD_GLUE_H_INCLUDE
#define TICD_GLUE_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "ticd/types/error_codes.h"
#include "ticd/types/message.h"

/**
 * @brief TICD framework receive message glue function
 * 
 * This function is the main entry point of inbound tasks. The framework is
 * responsible for allocating (or reserving) and cleaning up the memory for
 * the TICD message structure. The only job of the application writer is to
 * populate the source buffer and task buffer of the message structure. The
 * framework provides the @c ticd_message_populate_source and the
 * @c ticd_message_populate_task functions to help with the message structure
 * population.
 * 
 * The return value for this function is interpreted by the framework as the
 * number of bytes received when the return value is zero (0) or greater. If
 * the return value is negative, the framework assumes an error occured during
 * message reception and simply ignores the message.
 * 
 * This glue function is mandatory. The framework does not provide a weakly
 * defined implementation of this function.
 * 
 * @param[in,out] msg A pointer to a pre-allocated/reserved message structure
 * @return The number of bytes received or a negative number indicating an
 * application specific error
 */
int ticd_glue_rx_message(TicdMsg * const msg);


/**
 * @brief TICD framework transmit message glue function
 * 
 * This function is the main exit point of outbound task responses. The
 * framework is reponsible for allocating (or reserving) and cleaning up the
 * memory for the TICD message structure. The only job of the application
 * writer is to transmit the task field out the message structure on whatever
 * medium the application requires. The source field of the message structure
 * is the same source field that was copied into the task message structure
 * during task reception.
 * 
 * The framework does not attempt to do retransmission upon failures or any
 * kind of error handling with regards to transmission. It is up to the
 * application to handle any potential errors and/or retransmissions.
 * 
 * This glue function is mandatory. The framework does not provide a weakly
 * defined implementation of this function.
 * 
 * @param[in,out] msg A pointer to a pre-allocated/reserved message structure
 */
void ticd_glue_tx_message(const TicdMsg * const msg);


/**
 * @brief TICD framework application reset action glue function
 * 
 * The TICD framework manages the task handling for the reset task, but to
 * maintain flexibility across applications, the framework makes no attempt
 * to actually implement a system reset. Instead, the framework will call this
 * user provided function during task execution of a reset task. The response
 * to the reset task is transmitted after this function returns. It is the 
 * responsibility of the application writer to transmit the response (when
 * required) if this function is designed to not return.
 * 
 * This glue function is optional. The framework provides a weakly defined
 * implementation of this function that does nothing and returns.
 */
void ticd_glue_reset_action(void);


/**
 * @brief TICD framework application is ready glue function
 * 
 * Some applications may require additional initialization data from an
 * external source or have a long running background procedure that prevents
 * the processing of tasks. The framework provides a gating facility for
 * rejecting messages when in these not ready states by calling this user
 * provided function. Application writers can signal that the application is
 * ready for inbound tasks by returning the @c TICD_SUCCESS value. All other
 * values are interpreted as the application is not ready to accept inbound
 * tasks.
 * 
 * Since additional initialization data may come in the form of a task, the
 * gating functionality that uses this method to accept or reject tasks can
 * be bypassed by implementing the @c ticd_glue_task_can_bypass_ready.
 * See the documentation for @c ticd_glue_task_can_bypass_ready for
 * more information.
 * 
 * This glue function is optional. The framework provides a weakly defined
 * implementation of this function that always signals the application is 
 * ready.
 * 
 * @return @ref TICD_SUCCESS application can handle tasks
 * @return @ref TICD_FAILURE application is not in a state to handle tasks
 */
TicdErr ticd_glue_application_ready(void);


/**
 * @brief TICD framework application task ID can bypass ready state function
 * 
 * Whenever an application uses a ready state gate, it may still require some
 * application specific task ID's to execute when in a not ready state. The
 * framework will call this function after checking its internal list of ready
 * bypass task ID's to determine if the task should be accepted or rejected.
 * To allow the given test ID to bypass the gate, the function must return
 * @c TICD_SUCCESS. All other return values are interpreted as the task ID must
 * wait until the application is ready before execution.
 * 
 * This glue function is optional. The framework provides a weakly defined
 * implementation of this function that always signals that the task ID can NOT
 * bypass the ready gate
 * 
 * @param[in] task_id TICD task ID
 * 
 * @return @ref TICD_SUCCESS \p task_id can bypass ready state gating
 * @return @ref TICD_FAILURE \p task_id must obey the task ID gating logic
 */
TicdErr ticd_glue_task_can_bypass_ready(uint8_t task_id);


#ifdef __cplusplus
}
#endif

#endif /* TICD_GLUE_H_INCLUDE */