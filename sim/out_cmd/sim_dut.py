import cocotb
import os
import sys

from cocotb.clock import Clock
from cocotb.triggers import RisingEdge

# Import files from the packages directory under the sim folder of the
# project.
cwd  = os.path.abspath(os.path.curdir)
pkgs = os.path.join(cwd, "..", "packages")
sys.path.insert(0, os.path.abspath(pkgs))

import task_icd_pkg

class SimDut():

    # Valid length of an output task
    VALID_LEN = task_icd_pkg.HEADER_BYTES + 4

    # Minimum valid output
    OUT_MIN = 0

    # Maximum valid output
    OUT_MAX = 31

    # Execution timeout cycle count
    __TIMEOUT = 1000

    # Maximum amount of timeout latency before the simulation
    # asserts.
    __MAX_LATENCY = __TIMEOUT * 2

    def __init__(self, dut):
        self.__dut = dut
        cocotb.start_soon(Clock(self.__dut.clk, 2, units="ps").start())

    async def act(self, length, output, *, aso_out_valid_latency = 0, force_timeout = False):

        # Add a timeout amount of time to aso_out_valid latency when forcing a
        # timeout condition. Make sure the value isn't bigger than the
        # MAX_LATENCY value.
        if force_timeout:
            new_latency = aso_out_valid_latency + self.__TIMEOUT + 1
            aso_out_valid_latency = min(self.__MAX_LATENCY - 1, new_latency)

        await self.__strobe_cmd(length, output)

        latency  = 0
        cmd_data = list()

        while not self.__dut.resp_valid.value:

            if latency >= self.__MAX_LATENCY:
                assert 0, "DUT took too long to respond"
            elif latency > aso_out_valid_latency:
                cmd_data.append(self.__sink_cmd_word())

            latency += 1
            await self.wait_cycles()

        # Filter out None values without removing 0's
        cmd_data = [d for d in cmd_data if d is not None]

        return (self.__dut.resp.value, cmd_data)


    async def reset(self, cycles = 1):
        self.__dut.rst.value           = 1
        self.__dut.task_valid.value    = 0
        self.__dut.len_bytes.value     = 0
        self.__dut.out_strobe.value    = 0
        self.__dut.aso_cmd_ready.value = 0
        await self.wait_cycles(cycles)
        self.__dut.rst.value = 0
        await self.wait_cycles(1)


    async def wait_cycles(self, cycles = 1):
        for _ in range(cycles):
            await RisingEdge(self.__dut.clk)


    def __sink_cmd_word(self):
        cmd_data = None

        if self.__dut.aso_cmd_valid.value and self.__dut.aso_cmd_ready.value:
            cmd_data = self.__dut.aso_cmd_data.value
            self.__dut.aso_cmd_ready.value = 0
        else:
            self.__dut.aso_cmd_ready.value = 1

        return cmd_data


    async def __strobe_cmd(self, length, output):
        self.__dut.task_valid.value = 1
        self.__dut.len_bytes.value  = length
        self.__dut.out_strobe.value = output
        await self.wait_cycles()
        self.__dut.task_valid.value = 0