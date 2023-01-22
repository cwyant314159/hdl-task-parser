#pragma once

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "tb_out_cmd.h"
#include "tb_out_cmd_task_icd_pkg.h"

namespace Sim {

class Dut {

public:
    Dut();
    ~Dut();
    vluint64_t get_fail_count();
    void task(vluint32_t out, vluint32_t len, uint32_t cmd_ready_latency = 0);
    void exeErrorTest();
    void invalidLenTest();
    void outputTest();
    void reset(vluint64_t cycles = 2);
    void wait_cycles(vluint64_t cycles);

private:

    static constexpr vluint32_t VALID_LEN = tb_out_cmd_task_icd_pkg::HEADER_BYTES + 4;
    static constexpr vluint32_t OUT_MAX   = 31;
    static constexpr uint32_t   TIMEOUT   = 100000;

    tb_out_cmd    mDut;
    VerilatedVcdC mTrace;
  
    vluint64_t mSimTime    = 0;
    vluint64_t mPosEdgeCnt = 0;
    vluint64_t mTestCount  = 0;
    vluint64_t mFailCount  = 0;

    constexpr vluint32_t expected_resp(vluint32_t out, vluint32_t len, uint32_t latency);
    constexpr vluint32_t expected_cmd(vluint32_t out);
    void tick();
};

} // namespace Sim
