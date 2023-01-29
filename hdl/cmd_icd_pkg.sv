package cmd_icd_pkg;

/* verilator lint_off UNUSEDPARAM */
// MSB and LSB of fields in the command word
parameter int CMD_ID_LSB /*verilator public*/ = 'd28;
parameter int CMD_ID_MSG /*verilator public*/ = 'd31;

parameter int BANK_CMD_EN_LSB        /*verilator public*/ = 'd0;
parameter int BANK_CMD_EN_MSB        /*verilator public*/ = 'd3;
parameter int BANK_CMD_RESERVED0_LSB /*verilator public*/ = 'd4;
parameter int BANK_CMD_RESERVED0_MSB /*verilator public*/ = 'd7;
parameter int BANK_CMD_VAL_LSB       /*verilator public*/ = 'd8;
parameter int BANK_CMD_VAL_MSB       /*verilator public*/ = 'd15;
parameter int BANK_CMD_RESERVED1_LSB /*verilator public*/ = 'd16;
parameter int BANK_CMD_RESERVED1_MSB /*verilator public*/ = 'd27;

parameter int OUT_CMD_LSB          /*verilator public*/ = 'd0;
parameter int OUT_CMD_MSB          /*verilator public*/ = 'd4;
parameter int OUT_CMD_RESERVED_LSB /*verilator public*/ = 'd5;
parameter int OUT_CMD_RESERVED_MSB /*verilator public*/ = 'd27;
/*lint_on*/

// Message task id's
typedef enum logic[3:0] {
    CMD_ID_BANK = 4'b0000,
    CMD_ID_OUT  = 4'b0001
} cmd_id_t /*verilator public*/;

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
