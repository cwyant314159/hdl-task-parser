import cocotb
import os
import sys

from cocotb.clock import Clock
from cocotb.triggers import RisingEdge
from typing import List
from typing import Tuple

# Import files from the packages directory under the sim folder of the
# project.
cwd  = os.path.abspath(os.path.curdir)
pkgs = os.path.join(cwd, "..", "packages")
sys.path.insert(0, os.path.abspath(pkgs))

import task_icd_pkg

class SimDut():

    __PAYLOAD_WORDS_PER_TASK: int = 2

    VALID_TASK_LEN: List[int] = [
        task_icd_pkg.HEADER_BYTES + (4 * __PAYLOAD_WORDS_PER_TASK * 1),
        task_icd_pkg.HEADER_BYTES + (4 * __PAYLOAD_WORDS_PER_TASK * 2),
        task_icd_pkg.HEADER_BYTES + (4 * __PAYLOAD_WORDS_PER_TASK * 3),
        task_icd_pkg.HEADER_BYTES + (4 * __PAYLOAD_WORDS_PER_TASK * 4),
    ]

    # Bank enable field limits
    BANK_EN_MIN: int = 0
    BANK_EN_MAX: int = 15

    # Bank value field limits
    BANK_VAL_MIN: int = 0
    BANK_VAL_MAX: int = 255

    # Execution timeout cycle count
    __TIMEOUT: int = 1000

    # Maximum amount of timeout latency before the simulation
    # asserts.
    __MAX_LATENCY: int = __TIMEOUT * 2

    def __init__(self, dut):
        self.__dut = dut
        cocotb.start_soon(Clock(self.__dut.clk, 2, units="ps").start())

    async def act(self, task_list: List[Tuple[int, int]], *, aso_out_valid_latency:int = 0, force_timeout: bool = False) -> Tuple[int, List[int]]:

        # Add a timeout amount of time to aso_out_valid latency when forcing a
        # timeout condition. Make sure the value isn't bigger than the
        # MAX_LATENCY value. Since there are several clock cycles needed to
        # propagate data to the point where the timeout counter starts running
        # a little bit extra is added to the new latency value.
        if force_timeout:
            new_latency = aso_out_valid_latency + self.__TIMEOUT + 1 + 20
            aso_out_valid_latency = min(self.__MAX_LATENCY - 1, new_latency)

        await self.__strobe_cmd(task_list)

        latency  = 0
        cmd_data = list()

        while not self.__dut.resp_valid.value:

            if latency >= self.__MAX_LATENCY:
                assert 0, "DUT took too long to respond"
            elif latency > aso_out_valid_latency:
                cmd_data.append(self.__sink_cmd_word())

            latency += 1
            await self.wait_cycle()

        # Filter out None values without removing 0's
        cmd_data = [d for d in cmd_data if d is not None]

        return (self.__dut.resp.value, cmd_data)


    async def reset(self, cycles:int = 1):
        self.__dut.rst.value           = 1
        self.__dut.task_valid.value    = 0
        self.__dut.len_bytes.value     = 0
        self.__dut.bank0.value         = 0
        self.__dut.val0.value          = 0
        self.__dut.bank1.value         = 0
        self.__dut.val1.value          = 0
        self.__dut.bank2.value         = 0
        self.__dut.val2.value          = 0
        self.__dut.bank3.value         = 0
        self.__dut.val3.value          = 0
        self.__dut.aso_cmd_ready.value = 0
        await self.wait_cycle(cycles)
        self.__dut.rst.value = 0
        await self.wait_cycle(1)


    async def wait_cycle(self, cycles:int = 1):
        for _ in range(cycles):
            await RisingEdge(self.__dut.clk)


    def __sink_cmd_word(self) -> int:
        cmd_data = None

        if self.__dut.aso_cmd_valid.value and self.__dut.aso_cmd_ready.value:
            cmd_data = self.__dut.aso_cmd_data.value
            self.__dut.aso_cmd_ready.value = 0
        else:
            self.__dut.aso_cmd_ready.value = 1

        return cmd_data


    async def __strobe_cmd(self, task_list: List[Tuple[int, int]]):
        self.__dut.task_valid.value = 1
        
        # Determine the tasks length based on the length of the task list. Task
        # entries that do not have a DUT input are ignored.
        length = task_icd_pkg.HEADER_BYTES + (4 * self.__PAYLOAD_WORDS_PER_TASK * len(task_list))


        self.__dut.len_bytes.value  = length

        if len(task_list) >= 1:
            en, val = task_list[0]
            self.__dut.bank0.value = en
            self.__dut.val0.value  = val

        if len(task_list) >= 2:
            en, val = task_list[1]
            self.__dut.bank1.value = en
            self.__dut.val1.value  = val

        if len(task_list) >= 3:
            en, val = task_list[2]
            self.__dut.bank2.value = en
            self.__dut.val2.value  = val

        if len(task_list) >= 4:
            en, val = task_list[3]
            self.__dut.bank3.value = en
            self.__dut.val3.value  = val

        await self.wait_cycle()
        self.__dut.task_valid.value = 0