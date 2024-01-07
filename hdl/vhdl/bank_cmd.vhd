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
        bank0      : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        val0       : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        bank1      : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        val1       : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        bank2      : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        val2       : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        bank3      : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        val3       : IN STD_LOGIC_VECTOR(31 DOWNTO 0);

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

    -- Array type used to hold data for individual task commands
    TYPE WordArray IS ARRAY (0 TO 3) OF UNSIGNED(31 DOWNTO 0);

    CONSTANT BANK_CMD_ID   : UNSIGNED(3 DOWNTO 0) := "0000";
    
    CONSTANT MAX_TIMEOUT   : POSITIVE := 1000; -- clock ticks
    
    CONSTANT WORDS_PER_CMD : POSITIVE := 2;
    CONSTANT TASK_1_BYTES  : UNSIGNED(31 DOWNTO 0) := TO_UNSIGNED(HEADER_BYTES + (4 * WORDS_PER_CMD * 1), len_bytes'LENGTH);
    CONSTANT TASK_2_BYTES  : UNSIGNED(31 DOWNTO 0) := TO_UNSIGNED(HEADER_BYTES + (4 * WORDS_PER_CMD * 2), len_bytes'LENGTH);
    CONSTANT TASK_3_BYTES  : UNSIGNED(31 DOWNTO 0) := TO_UNSIGNED(HEADER_BYTES + (4 * WORDS_PER_CMD * 3), len_bytes'LENGTH);
    CONSTANT TASK_4_BYTES  : UNSIGNED(31 DOWNTO 0) := TO_UNSIGNED(HEADER_BYTES + (4 * WORDS_PER_CMD * 4), len_bytes'LENGTH);
    
    CONSTANT BANK_EN_MAX   : UNSIGNED(31 DOWNTO 0) := TO_UNSIGNED(15,  bank0'LENGTH);
    CONSTANT BANK_VAL_MAX  : UNSIGNED(31 DOWNTO 0) := TO_UNSIGNED(255, val0'LENGTH);

    TYPE State IS (IDLE, VALIDATE_LEN, VALIDATE_PAYLOAD, SRC);
    SIGNAL curr_state, next_state : State;

    SIGNAL next_aso_cmd_valid : STD_LOGIC;
    SIGNAL next_aso_cmd_data  : STD_LOGIC_VECTOR(31 DOWNTO 0);
    SIGNAL next_resp_valid    : STD_LOGIC;
    SIGNAL next_resp          : STD_LOGIC_VECTOR(31 DOWNTO 0);

    SIGNAL len,         next_len         : UNSIGNED(31 DOWNTO 0);
    SIGNAL bank,        next_bank        : WordArray;
    SIGNAL val,         next_val         : WordArray;
    SIGNAL cmd_idx,     next_cmd_idx     : NATURAL;
    SIGNAL cmd_limit,   next_cmd_limit   : NATURAL;
    SIGNAL timeout_cnt, next_timeout_cnt : NATURAL;

    SIGNAL cmd_data : WordArray;

BEGIN

     -- Form the output command from the incoming task data. We don't bother
     -- creating a register for this data bus since most of it is constant
     -- data. The only non-constant data is already registered. This bus does
     -- not need to be held at 0 when not in use since it has a valid signal
     -- associated with it.
    cmd_builder: FOR i in WordArray'LOW to WordArray'HIGH GENERATE
        cmd_data(i)(31 DOWNTO 28) <= BANK_CMD_ID;
        cmd_data(i)(27 DOWNTO 16) <= (OTHERS => '0');
        cmd_data(i)(15 DOWNTO  8) <= val(i)(7 DOWNTO 0);
        cmd_data(i)(7  DOWNTO  4) <= (OTHERS => '0');
        cmd_data(i)(3  DOWNTO  0) <= bank(i)(3 DOWNTO 0);
    END GENERATE;

    state_machine: PROCESS (ALL)
    BEGIN
        -- Prevent latches with these default values.
        --
        -- NOTE:
        -- The next_state variable is not assigned a default This is
        -- intentional. The next_state variable should always be actively set
        -- to a certain state.
        next_len           <= len;
        next_bank          <= bank;
        next_val           <= val;
        next_cmd_idx       <= cmd_idx;
        next_cmd_limit     <= cmd_limit;
        next_timeout_cnt   <= timeout_cnt;
        next_aso_cmd_valid <= '0';
        next_aso_cmd_data  <= (OTHERS => '0');
        next_resp_valid    <= '0';
        next_resp          <= resp;

        CASE curr_state IS

            -- Wait for the valid task strobe. Once a strobe is detected
            -- register the length and out_strobe inputs for payload
            -- validation.
            WHEN IDLE =>
                IF task_valid = '1' THEN
                    next_state   <= VALIDATE_LEN;
                    next_len     <= UNSIGNED(len_bytes);
                    next_bank(0) <= UNSIGNED(bank0);
                    next_bank(1) <= UNSIGNED(bank1);
                    next_bank(2) <= UNSIGNED(bank2);
                    next_bank(3) <= UNSIGNED(bank3);
                    next_val(0)  <= UNSIGNED(val0);
                    next_val(1)  <= UNSIGNED(val1);
                    next_val(2)  <= UNSIGNED(val2);
                    next_val(3)  <= UNSIGNED(val3);
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
            WHEN VALIDATE_LEN =>
                next_cmd_idx <= 0;

                IF len = TASK_1_BYTES THEN
                    next_state     <= VALIDATE_PAYLOAD;
                    next_cmd_limit <= 1;
                ELSIF len = TASK_2_BYTES THEN
                    next_state     <= VALIDATE_PAYLOAD;
                    next_cmd_limit <= 2;
                ELSIF len = TASK_3_BYTES THEN
                    next_state     <= VALIDATE_PAYLOAD;
                    next_cmd_limit <= 3;
                ELSIF len = TASK_4_BYTES THEN
                    next_state     <= VALIDATE_PAYLOAD;
                    next_cmd_limit <= 4;
                ELSE
                    next_state      <= IDLE;
                    next_resp_valid <= '1';
                    next_resp       <= RESP_HEADER_INVALID;
                END IF;

            WHEN VALIDATE_PAYLOAD =>
                IF cmd_idx = cmd_limit THEN
                    next_state       <= SRC;
                    next_cmd_idx     <= 0;
                    next_timeout_cnt <= 0;
                ELSE
                    next_state <= VALIDATE_PAYLOAD;

                    IF bank(cmd_idx) > BANK_EN_MAX OR val(cmd_idx) > BANK_VAL_MAX THEN
                        next_state      <= IDLE;
                        next_resp_valid <= '1';
                        next_resp       <= RESP_PAYLOAD_INVALID;
                    END IF;

                    next_cmd_idx <= cmd_idx + 1;
                END IF;

             -- Stream the output command built from the incoming task. If
             -- MAX_TIMEOUT clock cycles pass before the command is streamed
             -- out, it is assumed that the bus is locked up and the response
             -- is set to EXE_ERROR.
            WHEN SRC =>
                next_timeout_cnt <= timeout_cnt + 1;

                IF cmd_idx = cmd_limit THEN
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
                    next_aso_cmd_data  <= STD_LOGIC_VECTOR(cmd_data(cmd_idx));

                    IF aso_cmd_valid = '1' AND aso_cmd_ready = '1' THEN
                        next_timeout_cnt <= 0;
                        next_cmd_idx     <= cmd_idx + 1;
                    END IF;
                END IF;

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

            len           <= next_len;
            bank          <= next_bank;
            val           <= next_val;
            cmd_idx       <= next_cmd_idx;
            cmd_limit     <= next_cmd_limit;
            timeout_cnt   <= next_timeout_cnt;
            aso_cmd_data  <= next_aso_cmd_data;
            resp          <= next_resp;
        END IF;
    END PROCESS;

END ARCHITECTURE;
-------------------------------------------------------------------------------
