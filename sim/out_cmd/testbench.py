import cocotb
import os
import sys

from sim_dut import SimDut

# Import files from the packages directory under the sim folder of the
# project.
pkgs = os.path.join(os.path.curdir, "..", "packages")
sys.path.insert(0, os.path.abspath(pkgs))

import task_icd_pkg
import cmd_icd_pkg


def expected_response(length, out):

    if length != SimDut.VALID_LEN:
        return task_icd_pkg.Status.HEADER_INVALID.value
    elif out < SimDut.OUT_MIN or out > SimDut.OUT_MAX:
        return task_icd_pkg.Status.PAYLOAD_INVALID.value
    else:
        return task_icd_pkg.Status.TASK_VALID.value


def expected_commands(length, out):

    if expected_response(length, out) != task_icd_pkg.Status.TASK_VALID.value:
        return  list()

    return [cmd_icd_pkg.task2out_cmd(out)]



@cocotb.test()
async def invalid_length_test(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    length = SimDut.VALID_LEN + 1
    out    = SimDut.OUT_MIN

    resp, cmds = await tb_dut.act(length, out)
    assert resp == expected_response(length, out)
    assert cmds == expected_commands(length, out)

@cocotb.test()
async def output_test(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    length = SimDut.VALID_LEN

    for out in range(0,100):
        resp, cmds = await tb_dut.act(length, out)
        assert resp == expected_response(length, out)
        assert cmds == expected_commands(length, out)

@cocotb.test()
async def timout_test(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    length = SimDut.VALID_LEN
    out    = SimDut.OUT_MIN

    expected_resp = task_icd_pkg.Status.EXE_ERROR.value

    resp, cmds = await tb_dut.act(length, out, force_timeout=True)
    assert resp == expected_resp
    assert cmds == []