package cmd_icd_pkg;

// NOTE:
//  Not all constants will be used in all packages. Linting for unused
//  parameters is turned off to prevent noisy warnings.

// Command word id's
typedef enum logic[3:0] {
    CMD_ID_OUT  = 4'b0001
}cmd_id_t;

// Command builder functions
function automatic logic[31:0] task2out_cmd (
    input logic[4:0] out
);
    return {CMD_ID_OUT, 23'd0, out[4:0]};
endfunction

endpackage
