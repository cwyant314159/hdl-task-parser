from enum import Enum

MAX_MSG_WORDS = 12
MAX_MSG_BYTES = MAX_MSG_WORDS * 12
HEADER_WORDS  = 4
HEADER_BYTES  = HEADER_WORDS * 4

LEN_IDX       = 0
SEQ_ID_IDX    = 1
TASK_ID_IDX   = 2
STATUS_IDX    = 3
PAYLOAD_0_IDX = 4

class TaskID(Enum):
    TASK_ID_BANK = 100
    TASK_ID_OUT  = 101

class Status(Enum):
    TASK_VALID      = 0
    HEADER_INVALID  = 1
    PAYLOAD_INVALID = 2
    EXE_ERROR       = 3