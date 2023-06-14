package cmd_icd_pkg;

// NOTE:
//  Not all constants will be used in all packages. Linting for unused
//  parameters is turned off to prevent noisy warnings.

/* verilator lint_off UNUSEDPARAM */
// MSB and LSB of fields in the command word
parameter int CMD_ID_LSB = 'd28;
parameter int CMD_ID_MSB = 'd31;

parameter int BANK_CMD_EN_LSB        = 'd0;
parameter int BANK_CMD_EN_MSB        = 'd3;
parameter int BANK_CMD_RESERVED0_LSB = 'd4;
parameter int BANK_CMD_RESERVED0_MSB = 'd7;
parameter int BANK_CMD_VAL_LSB       = 'd8;
parameter int BANK_CMD_VAL_MSB       = 'd15;
parameter int BANK_CMD_RESERVED1_LSB = 'd16;
parameter int BANK_CMD_RESERVED1_MSB = 'd27;

parameter int OUT_CMD_LSB          = 'd0;
parameter int OUT_CMD_MSB          = 'd4;
parameter int OUT_CMD_RESERVED_LSB = 'd5;
parameter int OUT_CMD_RESERVED_MSB = 'd27;
/*lint_on*/

// Command word id's
typedef enum logic[3:0] {
    CMD_ID_BANK = 4'b0000,
    CMD_ID_OUT  = 4'b0001
}cmd_id_t;

// Command builder functions
function automatic logic[31:0] task2out_cmd (
    input logic[4:0] out
);
    return {CMD_ID_OUT, 23'd0, out[4:0]};
endfunction

function automatic logic[31:0] task2bank_cmd(
    input logic[3:0] bank,
    input logic[7:0] val
);
    return {CMD_ID_BANK, 12'd0, val[7:0], 4'd0, bank[3:0]};
endfunction

endpackage
