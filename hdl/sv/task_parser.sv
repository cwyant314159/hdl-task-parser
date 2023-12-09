/*
 * Task parser main module 
 *
 * The main task parser module is responsible for sinking the inbound task
 * stream and distributing it to the low level task handlers. 
 *
 * Although this module is meant to only marshal tasks to their respective
 * handler, a length validations are performed at this level of abstraction to
 * limit the need for repetitive error checking/handling in the task handlers
 * themselves. 
 *
 * As inbound task words are streamed into this module, a running word count
 * is incremented. Using this word count, the number of streamed in bytes can
 * be computed and compared to the length field of the task header. If these
 * values do not match, the task is immediately nack'ed with a HEADER_INVALID
 * response status. Also tasks that are either shorter than the minimum or 
 * longer than the maximum message size are nack'ed with a HEADER_INVALID 
 * response status. Specific task lengths are verified within their associated
 * task handlers.
 *
 * All responses messages are returned to sender in accordance with the this
 * project's Task ICD. The response message is built using fields from the
 * inbound task and the response output of the task handlers. If an inbound
 * task is malformed such that sending a response is not possible there really
 * isn't much this module can do to cope other than do its best to respond.
 * For example, if not enough words are streamed in and the task ID is not
 * present, the response will be sent using what ever task ID is already in
 * the message's register. The basic idea is garbage in is garbage out.
 * Upstream modules have the responsiblity of ensuring that well formed task
 * messages are streamed to this module.
 */
module task_parser (

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

import task_icd_pkg::*;

// DATA TYPES
typedef enum logic[2:0] {
    IDLE,           // wait for start of packet on asi_task
    SNK,            // sink incoming words until EOP or error
    DISCARD,        // dump words that exceed max message size
    VALIDATE_HDR,   // validate header fields and strobe the handlers
    TASK_HANDLE,    // wait for response from handlers
    SRC_RESP        // source the response message 
} state_t;

// INTERNAL SIGNALS
logic task_snk_cycle;
logic resp_src_cycle;

logic[31:0] len_bytes;
logic[31:0]  task_id;

state_t curr_state, next_state;

logic        task_resp_valid;
logic [31:0] task_resp;

logic        bank_cmd_task_valid;
logic        bank_cmd_resp_valid;
logic [31:0] bank_cmd_resp;
logic        bank_cmd_aso_cmd_valid;
logic [31:0] bank_cmd_aso_cmd_data;

logic        out_cmd_task_valid;
logic        out_cmd_resp_valid;
logic [31:0] out_cmd_resp;
logic        out_cmd_aso_cmd_valid;
logic [31:0] out_cmd_aso_cmd_data;

logic       next_asi_task_ready;
logic       next_aso_resp_valid;
logic       next_aso_resp_sop;
logic       next_aso_resp_eop;
logic[31:0] next_aso_resp_data;

logic[31:0] word_count,         next_word_count;
logic[31:0] msg[MAX_MSG_WORDS], next_msg[MAX_MSG_WORDS];

/* 
 * When valid and ready are both high on a streaming interface, this is called
 * a sink or source cycle (depending on the direction) for that interface. 
 */
assign task_snk_cycle = asi_task_ready & asi_task_valid;
assign resp_src_cycle = aso_resp_ready & aso_resp_valid;

/*
 * These assignments are bascially overlays onto the received message from the
 * streaming interface. This means that these values are only valid after
 * completely sinking the message (i.e. validity is only ensured once the
 * validation states of the state machine are reached).
 */
assign len_bytes = msg[LEN_IDX];
assign task_id   = msg[TASK_ID_IDX];

/*
 * Since there are only two possible commands and the task ID is registered
 * until the task response is sent, we can use this simple conditional to set
 * the response and command output stream signals.
 */
 always_comb 
 begin 
    if (TASK_ID_OUT == task_id)
    begin
        task_resp_valid = out_cmd_resp_valid;
        task_resp       = out_cmd_resp;
        aso_cmd_valid   = out_cmd_aso_cmd_valid;
        aso_cmd_data    = out_cmd_aso_cmd_data;
    end
    else
    begin
        task_resp_valid = bank_cmd_resp_valid;
        task_resp       = bank_cmd_resp;
        aso_cmd_valid   = bank_cmd_aso_cmd_valid;
        aso_cmd_data    = bank_cmd_aso_cmd_data;
    end
 end

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
    next_asi_task_ready = '0;
    next_aso_resp_valid = '0;
    next_aso_resp_sop   = '0;
    next_aso_resp_eop   = '0;
    next_aso_resp_data  = '0;
    next_word_count     = word_count;
    next_msg            = msg;

    /*
     * These signals are purely combinational and do not need the _next
     * prefix.
     */
    bank_cmd_task_valid = '0;
    out_cmd_task_valid  = '0;

    case(curr_state)
        IDLE:
        begin
            next_asi_task_ready = '1;
            
            if(task_snk_cycle && asi_task_sop)
            begin
                next_state      = SNK;
                next_msg[0]     = asi_task_data;
                next_word_count = 'd1;
            end
            else
            begin
                next_state      = IDLE;
                next_word_count = '0;
            end
        end

        SNK:
        begin
            next_state          = SNK;
            next_asi_task_ready = '1;

            if(task_snk_cycle)
            begin
                next_msg[word_count] = asi_task_data;
                
                if(asi_task_eop)
                begin
                    next_state          = VALIDATE_HDR;
                    next_word_count     = '0;
                    next_asi_task_ready = '0;
                end
                else if ((MAX_MSG_WORDS - 'd1) == word_count)
                begin
                    next_state      = DISCARD;
                    next_word_count = '0;
                end
                begin
                    next_word_count = word_count + 'd1;
                end
            end
        end

        DISCARD:
        begin
            if (asi_task_eop)
            begin
                next_state           = SRC_RESP;
                next_msg[STATUS_IDX] = HEADER_INVALID;
                next_asi_task_ready  = '0;
            end
            else
            begin
                next_state          = DISCARD;
                next_asi_task_ready = '1;
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
             * deffered to the individual task processing blocks.
             */ 
            if({len_bytes[29:0], 2'b00} != word_count)
            begin
                next_state           = SRC_RESP;
                next_msg[STATUS_IDX] = HEADER_INVALID;
            end
            else if(len_bytes < HEADER_BYTES || len_bytes > MAX_MSG_BYTES)
            begin
                next_state           = SRC_RESP;
                next_msg[STATUS_IDX] = HEADER_INVALID;
            end
            else
            begin
                case(task_id)
                    TASK_ID_BANK:
                    begin
                        next_state          = TASK_HANDLE;
                        bank_cmd_task_valid = '1; 
                    end

                    TASK_ID_OUT:
                    begin
                        next_state         = TASK_HANDLE;
                        out_cmd_task_valid = '1;
                    end

                    default:
                    begin
                        next_state           = SRC_RESP;
                        next_msg[STATUS_IDX] = HEADER_INVALID;
                    end
                endcase
            end
        end

        TASK_HANDLE:
        begin
            next_state           = task_resp_valid ? SRC_RESP : TASK_HANDLE;
            next_msg[STATUS_IDX] = task_resp;
            next_word_count      = '0;
        end

        SRC_RESP:
        begin
            if(word_count == HEADER_WORDS)
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
    curr_state     <= (rst) ? IDLE           : next_state;
    word_count     <= (rst) ? '0             : next_word_count;
    msg            <= (rst) ? '{default: '0} : next_msg;
    asi_task_ready <= (rst) ? '0             : next_asi_task_ready;
    aso_resp_valid <= (rst) ? '0             : next_aso_resp_valid;
    aso_resp_sop   <= (rst) ? '0             : next_aso_resp_sop;
    aso_resp_eop   <= (rst) ? '0             : next_aso_resp_eop;
    aso_resp_data  <= (rst) ? '0             : next_aso_resp_data;
end

/* 
 * Task Handlers
 */
bank_cmd bank_cmd0(
    .clk                (clk),
    .rst                (rst),

    .task_valid         (bank_cmd_task_valid),
    .len_bytes          (len_bytes),
    .bank0              (msg[PAYLOAD_0_IDX + 'd0]),
    .val0               (msg[PAYLOAD_0_IDX + 'd1]),
    .bank1              (msg[PAYLOAD_0_IDX + 'd2]),
    .val1               (msg[PAYLOAD_0_IDX + 'd3]),
    .bank2              (msg[PAYLOAD_0_IDX + 'd4]),
    .val2               (msg[PAYLOAD_0_IDX + 'd5]),
    .bank3              (msg[PAYLOAD_0_IDX + 'd6]),
    .val3               (msg[PAYLOAD_0_IDX + 'd7]),
    
    .resp_valid         (bank_cmd_resp_valid),
    .resp               (bank_cmd_resp),

    .aso_cmd_ready      (aso_cmd_ready),
    .aso_cmd_valid      (bank_cmd_aso_cmd_valid),
    .aso_cmd_data       (bank_cmd_aso_cmd_data)
);

out_cmd out_cmd0(
    .clk                 (clk),
    .rst                 (rst),

    .task_valid          (out_cmd_task_valid),
    .len_bytes           (len_bytes),
    .out_strobe          (msg[PAYLOAD_0_IDX + 'd0]),
    
    .resp_valid          (out_cmd_resp_valid),
    .resp                (out_cmd_resp),

    .aso_cmd_ready       (aso_cmd_ready),
    .aso_cmd_valid       (out_cmd_aso_cmd_valid),
    .aso_cmd_data        (out_cmd_aso_cmd_data)
);

endmodule
