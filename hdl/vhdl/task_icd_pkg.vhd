-------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;
-------------------------------------------------------------------------------
PACKAGE task_icd_pkg IS 

    CONSTANT MAX_MSG_WORDS : NATURAL := 12;
    CONSTANT HEADER_WORDS  : NATURAL := 4;
    CONSTANT HEADER_BYTES  : NATURAL := HEADER_WORDS * 4;

    SUBTYPE TaskId IS STD_LOGIC_VECTOR(7 DOWNTO 0);
    CONSTANT TASK_ID_BANK : TaskId := STD_LOGIC_VECTOR(TO_UNSIGNED(100, TaskId'LENGTH));
    CONSTANT TASK_ID_OUT  : TaskId := STD_LOGIC_VECTOR(TO_UNSIGNED(101, TaskId'LENGTH));

    SUBTYPE ResponseCode IS STD_LOGIC_VECTOR(31 DOWNTO 0);
    CONSTANT RESP_TASK_VALID     : ResponseCode := STD_LOGIC_VECTOR(TO_UNSIGNED(0, ResponseCode'LENGTH));
    CONSTANT RESP_HEADER_INVALID : ResponseCode := STD_LOGIC_VECTOR(TO_UNSIGNED(1, ResponseCode'LENGTH));
    CONSTANT RESP_PAYLOAD_INVALID: ResponseCode := STD_LOGIC_VECTOR(TO_UNSIGNED(2, ResponseCode'LENGTH));
    CONSTANT RESP_EXE_ERROR      : ResponseCode := STD_LOGIC_VECTOR(TO_UNSIGNED(3, ResponseCode'LENGTH));
END PACKAGE;    
-------------------------------------------------------------------------------

--// Indices of message fields in an array.
--parameter int unsigned LEN_IDX       = 'd0;  // header
--parameter int unsigned SEQ_ID_IDX    = 'd1;  // header
--parameter int unsigned TASK_ID_IDX   = 'd2;  // header
--parameter int unsigned STATUS_IDX    = 'd3;  // header
--parameter int unsigned PAYLOAD_0_IDX = 'd4;  // application payload
--
