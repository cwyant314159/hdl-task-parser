package task_icd_pkg;

// NOTE:
//  Not all constants will be used in all packages. Linting for unused
//  parameters is turned off to prevent noisy warnings.

/* verilator lint_off UNUSEDPARAM */
// Maximum size of a valid message
parameter int MAX_MSG_WORDS = 'd12;
parameter int MAX_MSG_BYTES = MAX_MSG_WORDS * 'd4;
parameter int HEADER_WORDS  = 'd4;
parameter int HEADER_BYTES  = HEADER_WORDS * 'd4;

// Indices of message fields in an array.
parameter int LEN_IDX       = 'd0;  // header
parameter int SEQ_ID_IDX    = 'd1;  // header
parameter int TASK_ID_IDX   = 'd2;  // header
parameter int STATUS_IDX    = 'd3;  // header
parameter int PAYLOAD_0_IDX = 'd4;  // application payload
/*lint_on*/

// Message task id's
typedef enum logic[7:0] {
    TASK_ID_BANK = 'd100,
    TASK_ID_OUT  = 'd101
} task_id_t;

// Message status field values
typedef enum logic[31:0] {
    TASK_VALID      = 'd0,
    HEADER_INVALID  = 'd1,
    PAYLOAD_INVALID = 'd2,
    EXE_ERROR       = 'd3
} status_t;

endpackage
