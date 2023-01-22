#pragma once

#include <cstdint>
#include <verilated.h>

#include "dut.hpp"
#include "out_task.hpp"
#include "tb_out_cmd.h"

namespace Sim {

class OutCmdDut: public Dut<tb_out_cmd> {

public:
    OutCmdDut(std::string wave_file);
    ~OutCmdDut();
    void ExeErrorTest();
    void InvalidLenTest();
    void OutputTest();

private:
    std::uint8_t toggle_clk();
    void act(const OutTask& task);
    void reset(std::uint64_t cycles = 2);
};

} // namespace Sim
