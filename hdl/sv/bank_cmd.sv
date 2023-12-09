module bank_cmd (

    // clocking interface
    input  logic       clk,
    input  logic       rst,

    // task
    input  logic       task_valid,
    input  logic[31:0] len_bytes,
    input  logic[31:0] bank0,
    input  logic[31:0] val0,
    input  logic[31:0] bank1,
    input  logic[31:0] val1,
    input  logic[31:0] bank2,
    input  logic[31:0] val2,
    input  logic[31:0] bank3,
    input  logic[31:0] val3,
    
    // response
    output logic       resp_valid,
    output logic[31:0] resp,

    // command stream
    input  logic       aso_cmd_ready,
    output logic       aso_cmd_valid,
    output logic[31:0] aso_cmd_data
);

// IMPORTS
import cmd_icd_pkg::*;
import task_icd_pkg::*;

// MODULE CONSTANTS
localparam logic[31:0] MAX_TIMEOUT = 'd1000; // clock ticks
    
localparam logic[31:0] CMD_WORDS = 'd2;
localparam logic[31:0] TASK_1_BYTES = HEADER_BYTES + ('d4 * CMD_WORDS * 'd1);
localparam logic[31:0] TASK_2_BYTES = HEADER_BYTES + ('d4 * CMD_WORDS * 'd2);
localparam logic[31:0] TASK_3_BYTES = HEADER_BYTES + ('d4 * CMD_WORDS * 'd3);
localparam logic[31:0] TASK_4_BYTES = HEADER_BYTES + ('d4 * CMD_WORDS * 'd4);

localparam logic[31:0] BANK_ENABLE_MAX = 'd15;

localparam logic[31:0] BANK_VAL_MAX = 'd255;

// DATA TYPES
typedef enum logic[1:0] {
    IDLE,
    VALIDATE_LEN,
    VALIDATE_PAYLOAD,
    SRC
} state_t;

// INTERNAL SIGNALS
state_t curr_state, next_state;

logic[31:0] cmd_data[4];

logic       next_aso_cmd_valid;
logic[31:0] next_aso_cmd_data;
logic       next_resp_valid;
logic[31:0] next_resp;

logic[31:0] len,         next_len;
logic[31:0] bank[4],     next_bank[4];
logic[31:0] val[4],      next_val[4];
logic[31:0] cmd_idx,     next_cmd_idx;
logic[31:0] cmd_limit,   next_cmd_limit;
logic[31:0] timeout_cnt, next_timeout_cnt;

/*
 * Form the output command from the incoming task data. We don't bother
 * creating a register for this data bus since most of it is constant data.
 * The only non-constant data is already registered. This bus does not need to
 * be held at 0 when not in use since it has a valid signal associated with
 * it.
 */
always_comb
begin
    for(int i = 0; i < 4; i++)
    begin
        cmd_data[i] = task2bank_cmd(bank[i][3:0], val[i][7:0]);
    end
end

always_comb
begin
    /* 
     * Prevent latches with these default values.
     *
     * NOTE:
     * The next_state variable is not assigned a default. This is
     * intentional. The next_state variable should alwasy be actively set to
     * a certain state.
     */
    next_len           = len;
    next_bank          = bank;
    next_val           = val;
    next_cmd_idx       = cmd_idx;
    next_cmd_limit     = cmd_limit;
    next_timeout_cnt   = timeout_cnt;
    next_aso_cmd_valid = '0;
    next_aso_cmd_data  = '0;
    next_resp_valid    = '0;
    next_resp          = resp;

    case(curr_state)
        
        /*
         * Wait for the valid task strobe. Once a strobe is detected register
         * the length and out_strobe inputs for payload validation.
         */ 
        IDLE:
        begin
            if(task_valid)
            begin
                next_state   = VALIDATE_LEN;
                next_len     = len_bytes;
                next_bank[0] = bank0;
                next_bank[1] = bank1;
                next_bank[2] = bank2;
                next_bank[3] = bank3;
                next_val[0]  = val0;
                next_val[1]  = val1;
                next_val[2]  = val2;
                next_val[3]  = val3;
            end
            else
            begin
                next_state = IDLE;
            end
        end

        /*
         * Verify that the length is equal to this commands message length
         * (including the header), and the output strobe value is within the
         * valid range. The lower bound is not checked since it is 0. If any
         * check is failed, the response is set to PAYLOAD_INVALID, and the
         * state machine jumps back to IDLE. If all checks are passed, the
         * timeout counter is cleared and the state machine jumps to the
         * streaming source state.
         */
        VALIDATE_LEN:
        begin
            next_cmd_idx = 'd0;

            if(len == TASK_1_BYTES)
            begin
                next_state     = VALIDATE_PAYLOAD;
                next_cmd_limit = 'd1;
            end
            else if(len == TASK_2_BYTES)
            begin
                next_state     = VALIDATE_PAYLOAD;
                next_cmd_limit = 'd2;
            end
            else if(len == TASK_3_BYTES)
            begin
                next_state     = VALIDATE_PAYLOAD;
                next_cmd_limit = 'd3;
            end
            else if(len == TASK_4_BYTES)
            begin
                next_state     = VALIDATE_PAYLOAD;
                next_cmd_limit = 'd4;
            end
            else
            begin
                next_state      = IDLE;
                next_resp_valid = '1;
                next_resp       = HEADER_INVALID;
            end
        end

        VALIDATE_PAYLOAD:
        begin
            if(cmd_idx == cmd_limit)
            begin
                next_state       = SRC;
                next_cmd_idx     = 'd0;
                next_timeout_cnt = 'd0;
            end
            else
            begin
                next_state = VALIDATE_PAYLOAD;

                if(bank[cmd_idx] > BANK_ENABLE_MAX || val[cmd_idx] > BANK_VAL_MAX)
                begin
                    next_state      = IDLE;
                    next_resp_valid = '1;
                    next_resp       = PAYLOAD_INVALID;
                end
                
                next_cmd_idx = cmd_idx + 'd1;
            end
        end

        /*
         * Stream the output command built from the incoming task. If
         * MAX_TIMOUT clock cycles pass before the command is streamed out, it
         * is assumed that the bus is locked up and the response is set to
         * EXE_ERROR. 
         */
        SRC:
        begin
            next_timeout_cnt = timeout_cnt + 'd1;

            if(cmd_idx == cmd_limit)
            begin
                next_state         = IDLE;
                next_resp_valid    = '1;
                next_resp          = TASK_VALID;
            end
            else if(MAX_TIMEOUT == timeout_cnt)
            begin
                next_state      = IDLE;
                next_resp_valid = '1;
                next_resp       = EXE_ERROR;
            end
            else
            begin
                next_state         = SRC;
                next_aso_cmd_valid = '1;
                next_aso_cmd_data  = cmd_data[cmd_idx];

                if(aso_cmd_valid & aso_cmd_ready)
                begin
                    next_timeout_cnt = 'd0;
                    next_cmd_idx     = cmd_idx + 'd1;
                end
            end
        end
    endcase
end

always_ff @ (posedge clk or posedge rst)
begin
    curr_state     <= (rst) ? IDLE          : next_state;
    len            <= (rst) ? '0            : next_len;
    bank           <= (rst) ? '{default:'0} : next_bank;
    val            <= (rst) ? '{default:'0} : next_val;
    cmd_idx        <= (rst) ? '0            : next_cmd_idx;
    cmd_limit      <= (rst) ? '0            : next_cmd_limit;
    timeout_cnt    <= (rst) ? '0            : next_timeout_cnt;
    aso_cmd_valid  <= (rst) ? '0            : next_aso_cmd_valid;
    aso_cmd_data   <= (rst) ? '0            : next_aso_cmd_data;
    resp_valid     <= (rst) ? '0            : next_resp_valid;
    resp           <= (rst) ? TASK_VALID    : next_resp;
end

endmodule
