#include "dut.hpp"

#include <iostream>

#include "tb_out_cmd_task_icd_pkg.h"
#include "tb_out_cmd_cmd_icd_pkg.h"

namespace Sim {

Dut::Dut()
{
  Verilated::traceEverOn(true);
  mDut.trace(&mTrace, 5);
  mTrace.open("waveform.vcd");

  std::cout << "=== SIMULATION BEGIN ===\n";
}

Dut::~Dut()
{
  std::cout << "=== SIMULATION END ===\n";
  std::cout << "Tests: " << mTestCount << '\n';
  std::cout << "Fails: " << mFailCount << '\n';
  mDut.final();
  mTrace.close();
}

vluint64_t Dut::GetFailCount()
{
  return mFailCount;
}


void Dut::Act(const OutTask& task)
{
    ++mTestCount;
    
    // compute expected outputs
    const vluint32_t e_resp = task.GetResp();
    const vluint32_t e_cmd  = task.GetCmd();

    // strobe command into the DUT
    mDut.len_bytes  = task.len;
    mDut.out_strobe = task.out;
    mDut.task_valid = 1;
    WaitCycles(1);
    mDut.task_valid = 0;

    // prep the output stream for potential command and wait
    // for response
    vluint32_t a_cmd = 0xFFFFFFFF;
    mDut.aso_cmd_ready = 0;

    // some for loop abuse to handle the latency counter
    for(uint32_t latency = 0; 0 == mDut.resp_valid; ++latency) {
        
        // delay asserting ready
        if(latency > task.latency) {
            mDut.aso_cmd_ready = 1;
        }

        // capture the command data
        if (mDut.aso_cmd_ready && mDut.aso_cmd_valid) {
            a_cmd = mDut.aso_cmd_data;
        }

        WaitCycles(1);
    }

    // de-assert stream strobe
    mDut.aso_cmd_ready = 0;
    WaitCycles(1);

    // collect the actual response
    const vluint32_t a_resp = mDut.resp;
    
    // validate results
    if (e_resp != a_resp) {
        mFailCount++;
        std::cout << "-------------------------------------------------\n";
        std::cout << "Response failed\n";
        std::cout << "out = " << task.out << " len = " << task.len << " latency = " << task.latency << '\n';
        std::cout << "Expected: " << e_resp << '\n';
        std::cout << "Actual  : " << a_resp << '\n';
        std::cout << "-------------------------------------------------\n";
    }

    if (e_resp == tb_out_cmd_task_icd_pkg::TASK_VALID && e_cmd != a_cmd) {
        mFailCount++;
        std::cout << "-------------------------------------------------\n";
        std::cout << "Command failed\n";
        std::cout << "out = " << task.out << " len = " << task.len << " latency = " << task.latency << '\n';
        std::cout << "Expected: " << e_cmd << '\n';
        std::cout << "Actual  : " << a_cmd << '\n';
        std::cout << "-------------------------------------------------\n";
    }
}

void Dut::ExeErrorTest()
{
    std::cout << "Execution Error Test\n";
    Reset();
    const OutTask task = OutTask(OutTask::OUT_MIN, OutTask::VALID_LEN, OutTask::TIMEOUT);
    Act(task);
}

void Dut::InvalidLenTest()
{
    std::cout << "Invalid Length Test\n";
    Reset(); 
    const OutTask task = OutTask(OutTask::OUT_MIN, OutTask::VALID_LEN + 1);
    Act(task);
}

void Dut::OutputTest()
{
    constexpr vluint32_t start = 0;
    constexpr vluint32_t end   = 100000;
    std::cout << "Output Test [" << start << " - " << end << "]\n";
    Reset();

    for (vluint32_t out = start; out <= end; ++out) {
        const OutTask task = OutTask(out);
        Act(task); 
    }
}

void Dut::Reset(vluint64_t cycles)
{
  mDut.rst           = 1;
  mDut.task_valid    = 0;
  mDut.len_bytes     = 0;
  mDut.out_strobe    = 0;
  mDut.aso_cmd_ready = 0;
  WaitCycles(2);
  
  mDut.rst = 0;
  WaitCycles(1);
}

void Dut::WaitCycles(vluint64_t cycles)
{
  const vluint64_t target_count = cycles + mPosEdgeCnt;

  while(target_count != mPosEdgeCnt) {
    tick();
  }
}

void Dut::tick()
{ 
  mDut.clk ^= 1;
  mDut.eval();

  if (1 == mDut.clk) {
    ++mPosEdgeCnt;
  }

  mTrace.dump(mSimTime);
  mSimTime++;
}

} // namespace Sim
