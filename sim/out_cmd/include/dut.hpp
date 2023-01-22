#pragma once

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "out_task.hpp"
#include "tb_out_cmd.h"
#include "tb_out_cmd_task_icd_pkg.h"

namespace Sim {

class Dut {

public:
    Dut();
    ~Dut();
    vluint64_t GetFailCount();
    void Act(const OutTask& task);
    void ExeErrorTest();
    void InvalidLenTest();
    void OutputTest();
    void Reset(vluint64_t cycles = 2);
    void WaitCycles(vluint64_t cycles);

private:

    tb_out_cmd    mDut;
    VerilatedVcdC mTrace;
  
    vluint64_t mSimTime    = 0;
    vluint64_t mPosEdgeCnt = 0;
    vluint64_t mTestCount  = 0;
    vluint64_t mFailCount  = 0;

    void tick();
};

} // namespace Sim
