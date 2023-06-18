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


def expected_response(task_list):

    # Only 4 tasks can be processed at one time. If more than 4 tasks are given
    # to the DUT, the length field in the header is invalid.
    if len(task_list) > 4:
        return task_icd_pkg.Status.HEADER_INVALID.value

    for t in task_list:
        en, val = t

        if en < SimDut.BANK_EN_MIN or en > SimDut.BANK_EN_MAX:
            return task_icd_pkg.Status.PAYLOAD_INVALID.value
        elif val < SimDut.BANK_VAL_MIN or val > SimDut.BANK_VAL_MAX:
            return task_icd_pkg.Status.PAYLOAD_INVALID.value

    return task_icd_pkg.Status.TASK_VALID.value


def expected_commands(task_list):

    if expected_response(task_list) != task_icd_pkg.Status.TASK_VALID.value:
        return  list()

    cmds = list()
    for t in task_list:
        en, val = t
        cmds.append(cmd_icd_pkg.task2bank_cmd(en, val))

    return cmds


@cocotb.test()
async def invalid_length_test(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    task_list = [
        (1, 1),
        (2, 2),
        (3, 3),
        (4, 4),
        (5, 6),
    ]

    resp, cmds = await tb_dut.act(task_list)
    assert resp == expected_response(task_list)
    assert cmds == expected_commands(task_list)

@cocotb.test()
async def bank_test1(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    for bank_en in range(0, 20):
        print(f"bank = {bank_en}, value = 0-300")
        for bank_value in range(0,300):
            task_list = [
                (bank_en, bank_value),
            ]

            resp, cmds = await tb_dut.act(task_list)
            assert resp == expected_response(task_list)
            assert cmds == expected_commands(task_list)


@cocotb.test()
async def bank_test2(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    for bank_en in range(0, 20):
        print(f"bank = {bank_en}, value = 0-300")
        for bank_value in range(0,300):
            task_list = [
                (bank_en, bank_value),
                (bank_en+1, bank_value+1),
            ]

            resp, cmds = await tb_dut.act(task_list)
            assert resp == expected_response(task_list)
            assert cmds == expected_commands(task_list)


@cocotb.test()
async def bank_test3(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    for bank_en in range(0, 20):
        print(f"bank = {bank_en}, value = 0-300")
        for bank_value in range(0,300):
            task_list = [
                (bank_en, bank_value),
                (bank_en+1, bank_value+1),
                (bank_en+2, bank_value+2),
            ]

            resp, cmds = await tb_dut.act(task_list)
            assert resp == expected_response(task_list)
            assert cmds == expected_commands(task_list)


@cocotb.test()
async def bank_test3(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    for bank_en in range(0, 20):
        print(f"bank = {bank_en}, value = 0-300")
        for bank_value in range(0,300):
            task_list = [
                (bank_en, bank_value),
                (bank_en+1, bank_value+1),
                (bank_en+2, bank_value+2),
                (bank_en+3, bank_value+3),
            ]

            resp, cmds = await tb_dut.act(task_list)
            assert resp == expected_response(task_list)
            assert cmds == expected_commands(task_list)


@cocotb.test()
async def timout_test(dut):
    tb_dut = SimDut(dut)
    await tb_dut.reset()

    task_list = [
        (15, 15),
        (10, 10)
    ]

    resp, cmds = await tb_dut.act(task_list, force_timeout=True)
    assert resp == task_icd_pkg.Status.EXE_ERROR.value
    assert cmds == []