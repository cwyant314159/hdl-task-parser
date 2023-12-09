/*
 * Output command task handler
 *
 * TODO long form documentation
 */
module out_cmd (

    // clocking interface
    input  logic       clk,
    input  logic       rst,

    // task
    input  logic       task_valid,
    input  logic[31:0] len_bytes,
    input  logic[31:0] out_strobe,
    
    // response
    output logic       resp_valid,
    output logic[31:0] resp,

    // command stream
    input  logic       aso_cmd_ready,
    output logic       aso_cmd_valid,
    output logic[31:0] aso_cmd_data
);

// IMPORTS
import task_icd_pkg::*;

// MODULE CONSTANTS
localparam logic[3:0]  OUT_CMD_ID = 4'b0001;

localparam logic[31:0] MAX_TIMEOUT = 'd1000; // clock ticks
    
localparam logic[31:0] CMD_WORDS = HEADER_WORDS + 'd1;
localparam logic[31:0] CMD_LEN   = CMD_WORDS * 'd4; // bytes

localparam logic[31:0] OUT_STROBE_MAX = 'd31;

// DATA TYPES
typedef enum logic[1:0] {
    IDLE,
    VALIDATE,
    SRC
} state_t;

// INTERNAL SIGNALS
state_t curr_state, next_state;

logic       next_aso_cmd_valid;
logic       next_resp_valid;
logic[31:0] next_resp;

logic[31:0] len,         next_len;
logic[31:0] out,         next_out;
logic[31:0] timeout_cnt, next_timeout_cnt;

/*
 * Form the output command from the incoming task data. We don't bother
 * creating a register for this data bus since most of it is constant data.
 * The only non-constant data is already registered. This bus does not need to
 * be held at 0 when not in use since it has a valid signal associated with
 * it.
 */
assign aso_cmd_data[31:28] = OUT_CMD_ID;
assign aso_cmd_data[27:5]  = '0;
assign aso_cmd_data[4:0]   = out[4:0];

always_comb
begin
    /* 
     * Prevent latches with these default values.
     *
     * NOTE:
     * The next_state variable is not assigned a default. This is
     * intentional. The next_state variable should always be actively set to
     * a certain state.
     */
    next_len           = len;
    next_out           = out;
    next_timeout_cnt   = timeout_cnt;
    next_aso_cmd_valid = '0;
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
                next_state = VALIDATE;
                next_len   = len_bytes;
                next_out   = out_strobe;
            end
            else
            begin
                next_state = IDLE;
            end
        end

        /*
         * Verify that the length is equal to this commands message length
         * (including the header), and the output strobe value is within the
         * valid range. The lower bound is not checked since it is 0. An
         * invalid length issues a HEADER_INVALID. All other failed checks
         * set the response to PAYLOAD_INVALID. Any failed check will put the
         * state machine back to IDLE. If all checks are passed, the timeout
         * counter is cleared and the state machine jumps to the streaming
         * source state.
         */
        VALIDATE:
        begin
            if(CMD_LEN != len)
            begin
                next_state      = IDLE;
                next_resp_valid = '1;
                next_resp       = HEADER_INVALID;
            end
            else if(out > OUT_STROBE_MAX)
            begin
                next_state      = IDLE;
                next_resp_valid = '1;
                next_resp       = PAYLOAD_INVALID;
            end
            else
            begin
                next_state       = SRC;
                next_timeout_cnt = '0;
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

            if(aso_cmd_valid & aso_cmd_ready)
            begin
                next_state         = IDLE;
                next_resp_valid    = '1;
                next_resp          = TASK_VALID;
            end
            else if(MAX_TIMEOUT == timeout_cnt)
            begin
                next_state         = IDLE;
                next_resp_valid    = '1;
                next_resp          = EXE_ERROR;
            end
            else
            begin
                next_state         = SRC;
                next_aso_cmd_valid = '1;
            end
        end

        /*
         * This case is un-reachable. It is only provided to appease the
         * linter. In the unfortunate case that it is reached, it simply jumps
         * back to the IDLE state.
         */
        default:
        begin
            next_state = IDLE;
        end
    endcase
end

always_ff @ (posedge clk or posedge rst)
begin
    curr_state     <= (rst) ? IDLE       : next_state;
    aso_cmd_valid  <= (rst) ? '0         : next_aso_cmd_valid;
    resp_valid     <= (rst) ? '0         : next_resp_valid;
    
    len            <= next_len;
    out            <= next_out;
    timeout_cnt    <= next_timeout_cnt;
    resp           <= next_resp;
end

endmodule
