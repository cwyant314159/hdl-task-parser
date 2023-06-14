from enum import Enum

CMD_ID_LSB = 28
CMD_ID_MSB = 31

BANK_CMD_EN_LSB        = 0
BANK_CMD_EN_MSB        = 3
BANK_CMD_RESERVED0_LSB = 4
BANK_CMD_RESERVED0_MSB = 7
BANK_CMD_VAL_LSB       = 8
BANK_CMD_VAL_MSB       = 15
BANK_CMD_RESERVED1_LSB = 16
BANK_CMD_RESERVED1_MSB = 27

OUT_CMD_LSB          = 0
OUT_CMD_MSB          = 4
OUT_CMD_RESERVED_LSB = 5
OUT_CMD_RESERVED_MSB = 27

class CommandId(Enum):
    CMD_ID_BANK = 0b0000
    CMD_ID_OUT  = 0b0001

def __bits2mask(msb, lsb, normalize = False):
    """
    Compute a bit mask given its MSB and LSB. If normalize is true the mask
    will be shifted such that the LSB of the mask is bit 0.
    """

    assert msb > lsb

    mask_msb = 1 << msb
    mask_msb |= mask_msb - 1

    if lsb > 0:
        mask_lsb = 1 << (lsb - 1)
        mask_lsb |= mask_lsb - 1
    else:
        mask_lsb = 0

    mask = mask_msb - mask_lsb

    if normalize:
        mask >> lsb

    return mask


def task2out_cmd(out):
    out_mask = __bits2mask(OUT_CMD_MSB, OUT_CMD_LSB, normalize=True)
    
    i = CommandId.CMD_ID_OUT.value << CMD_ID_LSB
    o = (out & out_mask )          << OUT_CMD_LSB
    
    return i | o


def task2bank_cmd(bank, value):
    bank_mask  = __bits2mask(BANK_CMD_EN_MSB, BANK_CMD_EN_LSB, normalize=True)
    value_mask = __bits2mask(BANK_CMD_VAL_MSB, BANK_CMD_VAL_LSB, normalize=True)

    i = CommandId.CMD_ID_BANK.value << CMD_ID_LSB
    b = (bank  & bank_mask)         << BANK_CMD_EN_LSB
    v = (value & value_mask)        << BANK_CMD_VAL_LSB

    return i | b | v