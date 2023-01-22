#include "out_cmd_dut.hpp"

#include <iostream>

#include "tb_out_cmd_task_icd_pkg.h"
#include "tb_out_cmd_cmd_icd_pkg.h"

namespace Sim {

OutCmdDut::OutCmdDut(std::string wave_file):
    Dut<tb_out_cmd>(wave_file)
{
    std::cout << "=== OUTPUT COMMAND SIMULATION BEGIN ===\n";
}

OutCmdDut::~OutCmdDut()
{
    std::cout << "=== OUTPUT COMMAND SIMULATION END ===\n";
    std::cout << "Tests: " << this->mTestCount << '\n';
    std::cout << "Fails: " << this->mFailCount << '\n';
}

void OutCmdDut::act(const OutTask& task)
{
    ++this->mTestCount;
    
    // compute expected outputs
    const vluint32_t e_resp = task.GetResp();
    const vluint32_t e_cmd  = task.GetCmd();

    // strobe command into the DUT
    this->mDut.len_bytes  = task.len;
    this->mDut.out_strobe = task.out;
    this->mDut.task_valid = 1;
    this->WaitCycles(1);
    this->mDut.task_valid = 0;

    // prep the output stream for potential command and wait
    // for response
    this->mDut.aso_cmd_ready = 0;
    vluint32_t a_cmd = 0xFFFFFFFF;

    // some for loop abuse to handle the latency counter
    for(uint32_t latency = 0; 0 == this->mDut.resp_valid; ++latency) {
        
        // delay asserting ready
        if(latency > task.latency) {
            this->mDut.aso_cmd_ready = 1;
        }

        // capture the command data
        if (this->mDut.aso_cmd_ready && this->mDut.aso_cmd_valid) {
            a_cmd = this->mDut.aso_cmd_data;
        }

        this->WaitCycles(1);
    }

    // de-assert stream strobe
    this->mDut.aso_cmd_ready = 0;
    this->WaitCycles(1);

    // collect the actual response
    const vluint32_t a_resp = this->mDut.resp;
    
    // validate results
    if (e_resp != a_resp) {
        ++this->mFailCount;
        std::cout << "-------------------------------------------------\n";
        std::cout << "Response failed\n";
        std::cout << "out = " << task.out << " len = " << task.len << " latency = " << task.latency << '\n';
        std::cout << "Expected: " << e_resp << '\n';
        std::cout << "Actual  : " << a_resp << '\n';
        std::cout << "-------------------------------------------------\n";
    }

    if (e_resp == tb_out_cmd_task_icd_pkg::TASK_VALID && e_cmd != a_cmd) {
        ++this->mFailCount;
        std::cout << "-------------------------------------------------\n";
        std::cout << "Command failed\n";
        std::cout << "out = " << task.out << " len = " << task.len << " latency = " << task.latency << '\n';
        std::cout << "Expected: " << e_cmd << '\n';
        std::cout << "Actual  : " << a_cmd << '\n';
        std::cout << "-------------------------------------------------\n";
    }
}

void OutCmdDut::ExeErrorTest()
{
    std::cout << "Execution Error Test\n";
    this->reset();
    const OutTask task = OutTask(OutTask::OUT_MIN, OutTask::VALID_LEN, OutTask::TIMEOUT);
    this->act(task);
}

void OutCmdDut::InvalidLenTest()
{
    std::cout << "Invalid Length Test\n";
    this->reset(); 
    const OutTask task = OutTask(OutTask::OUT_MIN, OutTask::VALID_LEN + 1);
    this->act(task);
}

void OutCmdDut::OutputTest()
{
    constexpr vluint32_t start = 0;
    constexpr vluint32_t end   = 100;
    std::cout << "Output Test [" << start << " - " << end << "]\n";
    this->reset();

    for (vluint32_t out = start; out <= end; ++out) {
        const OutTask task = OutTask(out);
        this->act(task); 
    }
}

void OutCmdDut::reset(vluint64_t cycles)
{
  this->mDut.rst           = 1;
  this->mDut.task_valid    = 0;
  this->mDut.len_bytes     = 0;
  this->mDut.out_strobe    = 0;
  this->mDut.aso_cmd_ready = 0;
  this->WaitCycles(2);
  
  this->mDut.rst = 0;
  this->WaitCycles(1);
}

std::uint8_t OutCmdDut::toggle_clk()
{
    this->mDut.clk ^= 1;
    return this->mDut.clk;
}

} // namespace Sim
