-------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;

LIBRARY work;
USE work.task_icd_pkg.all;
-------------------------------------------------------------------------------
ENTITY bank_cmd IS
    PORT(
        -- clocking interface
        clk : IN STD_LOGIC;
        rst : IN STD_LOGIC;

        -- task
        task_valid : IN STD_LOGIC;
        len_bytes  : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        out_strobe : IN STD_LOGIC_VECTOR(31 DOWNTO 0);

        -- response
        resp_valid : OUT STD_LOGIC;
        resp       : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);

        -- command stream
        aso_cmd_ready : IN  STD_LOGIC;
        aso_cmd_valid : OUT STD_LOGIC;
        aso_cmd_data  : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
    );
END ENTITY;
-------------------------------------------------------------------------------
ARCHITECTURE arch OF bank_cmd IS

    CONSTANT OUT_CMD_ID   : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0001";
    
    CONSTANT MAX_TIMEOUT   : POSITIVE := 1000; -- clock ticks
    
    CONSTANT WORDS_PER_CMD : POSITIVE := 1;
    CONSTANT TASK_BYTES    : UNSIGNED(31 DOWNTO 0) := TO_UNSIGNED(HEADER_BYTES + (4 * WORDS_PER_CMD), len_bytes'LENGTH);
    
    CONSTANT OUT_STROBE_MAX : UNSIGNED(31 DOWNTO 0) := TO_UNSIGNED(31,  out_strobe'LENGTH);

    TYPE State IS (IDLE, VALIDATE, SRC);
    SIGNAL curr_state, next_state : State;

    SIGNAL next_aso_cmd_valid : STD_LOGIC;
    SIGNAL next_resp_valid    : STD_LOGIC;
    SIGNAL next_resp          : STD_LOGIC_VECTOR(31 DOWNTO 0);

    SIGNAL len,         next_len         : UNSIGNED(31 DOWNTO 0);
    SIGNAL out_stb,     next_out_stb     : UNSIGNED(31 DOWNTO 0);
    SIGNAL timeout_cnt, next_timeout_cnt : NATURAL;

BEGIN

     -- Form the output command from the incoming task data. We don't bother
     -- creating a register for this data bus since most of it is constant
     -- data. The only non-constant data is already registered. This bus does
     -- not need to be held at 0 when not in use since it has a valid signal
     -- associated with it.
    aso_cmd_data(31 DOWNTO 28) <= OUT_CMD_ID;
    aso_cmd_Data(27 DOWNTO  5) <= (OTHERS => '0');
    aso_cmd_data(4  DOWNTO  0) <= STD_LOGIC_VECTOR(out_stb);

    state_machine: PROCESS (ALL)
    BEGIN
        -- Prevent latches with these default values.
        --
        -- NOTE:
        -- The next_state variable is not assigned a default This is
        -- intentional. The next_state variable should always be actively set
        -- to a certain state.
        next_len           <= len;
        next_out_stb       <= out_stb;
        next_timeout_cnt   <= timeout_cnt;
        next_aso_cmd_valid <= '0';
        next_resp_valid    <= '0';
        next_resp          <= resp;

        CASE curr_state IS

            -- Wait for the valid task strobe. Once a strobe is detected
            -- register the length and out_strobe inputs for payload
            -- validation.
            WHEN IDLE =>
                IF task_valid = '1' THEN
                    next_state   <= VALIDATE;
                    next_len     <= UNSIGNED(len_bytes);
                    next_out_stb <= UNSIGNED(out_strobe);
                ELSE
                    next_state <= IDLE;
                END IF;

             -- Verify that the length is equal to this commands message length
             -- (including the header), and the output strobe value is within
             -- the valid range. The lower bound is not checked since it is 0.
             -- If any check is failed, the response is set to PAYLOAD_INVALID,
             -- and the state machine jumps back to IDLE. If all checks are
             -- passed, the timeout counter is cleared and the state machine
             -- jumps to the streaming source state.
            WHEN VALIDATE =>
                IF len /= TASK_BYTES THEN
                    next_state      <= IDLE;
                    next_resp_valid <= '1';
                    next_resp       <= RESP_HEADER_INVALID;
                ELSIF out_stb > OUT_STROBE_MAX THEN
                    next_state      <= IDLE;
                    next_resp_valid <= '1';
                    next_resp       <= RESP_PAYLOAD_INVALID;
                ELSE
                    next_state       <= SRC;
                    next_timeout_cnt <= 0;
                END IF;

             -- Stream the output command built from the incoming task. If
             -- MAX_TIMEOUT clock cycles pass before the command is streamed
             -- out, it is assumed that the bus is locked up and the response
             -- is set to EXE_ERROR.
            WHEN SRC =>
                next_timeout_cnt <= timeout_cnt + 1;

                IF aso_cmd_valid = '1' AND aso_cmd_ready = '1' THEN
                    next_state      <= IDLE;
                    next_resp_valid <= '1';
                    next_resp       <= RESP_TASK_VALID;
                ELSIF MAX_TIMEOUT = timeout_cnt THEN
                    next_state      <= IDLE;
                    next_resp_valid <= '1';
                    next_resp       <= RESP_EXE_ERROR;
                ELSE
                    next_state         <= SRC;
                    next_aso_cmd_valid <= '1';
                END IF;

            -- This case is un-reachable. It is only provided to appease the
            -- linter. In the unfortunate case that it is reached, it simply
            -- jumps back to the IDLE state.
            WHEN OTHERS =>
                next_state <= IDLE;
        END CASE;
    END PROCESS;

    ff: PROCESS (clk)
    BEGIN
        IF RISING_EDGE(clk) THEN
            curr_state    <= IDLE WHEN rst = '1' ELSE next_state;
            aso_cmd_valid <= '0'  WHEN rst = '1' ELSE next_aso_cmd_valid;
            resp_valid    <= '0'  WHEN rst = '1' ELSE next_resp_valid;

            len         <= next_len;
            out_stb     <= next_out_stb;
            timeout_cnt <= next_timeout_cnt;
            resp        <= next_resp;
        END IF;
    END PROCESS;

END ARCHITECTURE;
-------------------------------------------------------------------------------
