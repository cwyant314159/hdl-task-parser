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


void OutCmdDut::ExeErrorTest()
{
    std::cout << "Execution Error Test\n";
    this->reset();
    this->act(this->OUT_MIN, this->VALID_LEN, this->TIMEOUT);
}

void OutCmdDut::InvalidLenTest()
{
    std::cout << "Invalid Length Test\n";
    this->reset(); 
    this->act(this->OUT_MIN, this->VALID_LEN+1);
}

void OutCmdDut::OutputTest()
{
    constexpr vluint32_t start = 0;
    constexpr vluint32_t end   = 100;
    std::cout << "Output Test [" << start << " - " << end << "]\n";
    this->reset();

    for (vluint32_t out = start; out <= end; ++out) {
        this->act(out); 
    }
}

void OutCmdDut::act(vluint32_t out, vluint32_t len, vluint32_t latency)
{
    ++this->mTestCount;
    
    // compute expected outputs
    const vluint32_t e_resp = this->get_resp(out, len, latency);
    const vluint32_t e_cmd  = this->get_cmd(out, len, latency);

    // strobe command into the DUT
    this->mDut.len_bytes  = len;
    this->mDut.out_strobe = out;
    this->mDut.task_valid = 1;
    this->WaitCycles(1);
    this->mDut.task_valid = 0;

    // prep the output stream for potential command and wait
    // for response
    this->mDut.aso_cmd_ready = 0;
    vluint32_t a_cmd = 0xFFFFFFFF;

    // some for loop abuse to handle the latency counter
    for(uint32_t l = 0; 0 == this->mDut.resp_valid; ++l) {
        
        // delay asserting ready
        if(l > latency) {
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
        std::cout << "out = " << out << " len = " << len << " latency = " << latency << '\n';
        std::cout << "Expected: " << e_resp << '\n';
        std::cout << "Actual  : " << a_resp << '\n';
        std::cout << "-------------------------------------------------\n";
    }

    if (e_resp == tb_out_cmd_task_icd_pkg::TASK_VALID && e_cmd != a_cmd) {
        ++this->mFailCount;
        std::cout << "-------------------------------------------------\n";
        std::cout << "Command failed\n";
        std::cout << "out = " << out << " len = " << len << " latency = " << latency << '\n';
        std::cout << "Expected: " << e_cmd << '\n';
        std::cout << "Actual  : " << a_cmd << '\n';
        std::cout << "-------------------------------------------------\n";
    }
}

constexpr vluint32_t OutCmdDut::get_resp(vluint32_t out, vluint32_t len, std::uint32_t latency) const noexcept
{
    if (len != VALID_LEN) { 
        return tb_out_cmd_task_icd_pkg::HEADER_INVALID;
    } else if (out > OUT_MAX) {
        return tb_out_cmd_task_icd_pkg::PAYLOAD_INVALID;
    } else if (latency > TIMEOUT) { 
        return tb_out_cmd_task_icd_pkg::EXE_ERROR;
    }

    return tb_out_cmd_task_icd_pkg::TASK_VALID;
}

constexpr vluint32_t OutCmdDut::get_cmd(vluint32_t out, vluint32_t len, std::uint32_t latency) const noexcept
{
    const vluint32_t resp = this->get_resp(out, len, latency);

    if (tb_out_cmd_task_icd_pkg::TASK_VALID != resp) {
        return 0xFFFFFFFF;
    }

    constexpr uint32_t id = tb_out_cmd_cmd_icd_pkg::CMD_ID_OUT <<
        tb_out_cmd_cmd_icd_pkg::CMD_ID_LSB;

    const uint32_t o = out << tb_out_cmd_cmd_icd_pkg::OUT_CMD_LSB; 
    
    return id | o;
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
