package cmd_icd_pkg;

// NOTE:
//  Not all constants will be used in all packages. Linting for unused
//  parameters is turned off to prevent noisy warnings.
/* verilator lint_off UNUSEDPARAM */

// MSB and LSB of fields in the command word
parameter int unsigned CMD_ID_LSB = 'd28;
parameter int unsigned CMD_ID_MSB = 'd31;

parameter int unsigned BANK_CMD_EN_LSB        = 'd0;
parameter int unsigned BANK_CMD_EN_MSB        = 'd3;
parameter int unsigned BANK_CMD_RESERVED0_LSB = 'd4;
parameter int unsigned BANK_CMD_RESERVED0_MSB = 'd7;
parameter int unsigned BANK_CMD_VAL_LSB       = 'd8;
parameter int unsigned BANK_CMD_VAL_MSB       = 'd15;
parameter int unsigned BANK_CMD_RESERVED1_LSB = 'd16;
parameter int unsigned BANK_CMD_RESERVED1_MSB = 'd27;

parameter int unsigned OUT_CMD_LSB          = 'd0;
parameter int unsigned OUT_CMD_MSB          = 'd4;
parameter int unsigned OUT_CMD_RESERVED_LSB = 'd5;
parameter int unsigned OUT_CMD_RESERVED_MSB = 'd27;

/*lint_on*/

// Command word id's
typedef enum logic[3:0] {
    CMD_ID_BANK = 4'b0000,
    CMD_ID_OUT  = 4'b0001
}cmd_id_t;

endpackage
