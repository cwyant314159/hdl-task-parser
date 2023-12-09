module st_parser (

    // clocking interface
    input logic        clk,
    input logic        rst,

    // task stream input
    output logic       asi_task_ready,
    input  logic       asi_task_valid,
    input  logic       asi_task_sop,
    input  logic       asi_task_eop,
    input  logic[31:0] asi_task_data,
    
    // response stream output
    input  logic       aso_resp_ready,
    output logic       aso_resp_valid,
    output logic       aso_resp_sop,
    output logic       aso_resp_eop,
    output logic[31:0] aso_resp_data,

    // output stream
    input  logic       aso_cmd_ready,
    output logic       aso_cmd_valid,
    output logic[31:0] aso_cmd_data
);

import icd_pkg::*;

// DATA TYPES
typedef enum logic[1:0] {
    IDLE,
    SNK,
    VALIDATE_HDR,
    TASK_HANDLE,
    SRC_CMD,
    SRC_RESP
} state_t;

// INTERNAL SIGNALS
logic task_snk_cycle;
logic resp_src_cycle;
logic cmd_src_cycle;

logic[31:0] len_bytes;
logic[7:0]  task_id;

state_t curr_state, next_state;

logic       next_asi_task_ready;
logic       next_aso_resp_valid;
logic       next_aso_resp_sop;
logic       next_aso_resp_eop;
logic[31:0] next_aso_resp_data;
logic       next_aso_cmd_valid;
logic[31:0] next_aso_cmd_data;

logic[31:0] word_cnt,           next_word_cnt;
logic[31:0] msg[MAX_MSG_WORDS], next_msg[MAX_MSG_WORDS];

/* 
 * When valid and ready are both high on a streaming interface, this is called
 * a sink or source cycle (depending on stream direction) on that interface. 
 */
assign task_snk_cycle = asi_task_ready & asi_task_valid;
assign resp_src_cycle = aso_resp_ready & aso_resp_valid;
assign cmd_src_cycle  = aso_cmd_ready  & aso_cmd_valid;

/*
 * These assignments are bascially overlays onto the received message from the
 * streaming interface. This means that these values are only valid after
 * completely sinking the message (i.e. validity is only ensured in the
 * validation states of the state machine).
 */
assign len_bytes = msg[LEN_IDX];
assign task_id   = msg[TASK_ID_IDX];

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
    next_asi_task_ready = '0;
    next_aso_resp_valid = '0;
    next_aso_resp_sop   = '0;
    next_aso_resp_eop   = '0;
    next_aso_resp_data  = '0;
    next_aso_cmd_valid  = '0;
    next_aso_cmd_data   = '0;
    next_word_cnt       = word_cnt;
    next_msg            = msg;

    case(curr_state)
        IDLE:
        begin
            if(task_snk_cycle && asi_task_sop)
            begin
                next_state         = SNK;
                next_msg[word_cnt] = asi_task_data;
                next_word_cnt      = word_cnt + 'd1;
            end
            else
            begin
                next_state          = IDLE;
                next_asi_task_ready = '1;
                next_word_cnt       = '0;
            end
        end

        SNK:
        begin
            next_state          = SNK;
            next_asi_task_ready = '1;

            if(task_snk_cycle)
            begin
                next_msg[word_cnt] = asi_task_data;
                
                if(asi_task_eop)
                begin
                    next_state          = PARSE;
                    next_asi_task_ready = '0;
                end
                else
                begin
                    next_word_cnt = (word_cnt == (MAX_MSG_SZ - 'd1)) ? word_cnt : word_cnt + 'd1;
                end
            end
        end

        VALIDATE_HDR:
        begin
            /* 
             * By default assume the task is valid and the next state is
             * going to source the task's resulting command.
             */
            next_state           = TASK_HANDLE;
            next_msg[STATUS_IDX] = TASK_VALID;

            /*
             * Validate the header fields. The first check ensures that the
             * length field matches the number of words received. This is
             * achieved by shifting the length field to the left by 2 (and
             * back filling with 0's). 
             *
             * The second check validates the minimum and maximum message
             * length possible for this application.
             *
             * If these checks pass, the payload is fowarded to a task handler
             * based off the header's task_id field. Payload verification is
             * deffered to the individual command processing.
             */ 
            if({len_bytes[29:0], 2'b00} != word_count)
            begin
                next_state           = SRC_RESP;
                next_msg[STATUS_IDX] = TASK_INVALID;
            end
            else if(len_bytes < HEADER_BYTES || len_bytes > MAX_MSG_BYTES)
            begin
                next_state           = SRC_RESP;
                next_msg[STATUS_IDX] = TASK_INVALID;
            end
            else
            begin
                case(task_id)
                    TASK_ID_BANK:
                    begin
                        // TODO strobe command block
                    end

                    TASK_ID_OUT:
                    begin
                        // TODO strobe command block
                    end

                    default:
                    begin
                        next_state           = SRC_RESP;
                        next_msg[STATUS_IDX] = TASK_INVALID;
                    end
                endcase
            end
        end

        TASK_HANDLE:
        begin
            // TODO wait for resp 
        end

        SRC_CMD:
        begin
            next_state         = (cmd_src_cycle) ? SRC_RESP : SRC_CMD;
            next_aso_cmd_valid = '1;
        end
    
        SRC_RESP:
        begin
            if(word_cnt == HEADER_WORDS)
            begin
                next_state          = IDLE;
                next_aso_resp_valid = '0;
            end
            else
            begin
                next_state          = SRC_RESP;
                next_aso_resp_valid = '1;
                next_aso_resp_data  = msg[word_count];
                next_aso_resp_sop   = ('0 == word_count);
                next_aso_resp_eop   = ((HEADER_WORDS - 'd1) == word_count);
                next_word_count     = (resp_src_cycle) ? word_count + 'd1 : word_count;
            end
        end
    endcase
end

always_ff @ (posedge clk or posedge rst)
begin
    curr_state     <= (rst) ? IDLE           : next_state;
    word_cnt       <= (rst) ? '0             : next_word_cnt;
    msg            <= (rst) ? '{default: '0} : next_msg;
    asi_task_ready <= (rst) ? '0             : next_asi_task_ready;
    aso_resp_valid <= (rst) ? '0             : next_aso_resp_valid;
    aso_resp_data  <= (rst) ? '0             : next_aso_resp_data;
    aso_cmd_valid  <= (rst) ? '0             : next_aso_cmd_valid;
    aso_cmd_data   <= (rst) ? '0             : next_aso_cmd_data;
end

endmodule
