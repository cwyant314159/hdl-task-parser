#include "sim_dut.hpp"

#include <iostream>

#include "tb_out_cmd_task_icd_pkg.h"
#include "tb_out_cmd_cmd_icd_pkg.h"

SimDut::SimDut()
{
  Verilated::traceEverOn(true);
  mDut.trace(&mTrace, 5);
  mTrace.open("waveform.vcd");

  std::cout << "=== SIMULATION BEGIN ===\n";
}

SimDut::~SimDut()
{
  std::cout << "=== SIMULATION END ===\n";
  std::cout << "Tests: " << mTestCount << '\n';
  std::cout << "Fails: " << mFailCount << '\n';
  mDut.final();
  mTrace.close();
}

vluint64_t SimDut::get_fail_count()
{
  return mFailCount;
}


void SimDut::task(vluint32_t out, vluint32_t len, uint32_t cmd_ready_latency)
{
    ++mTestCount;
    
    // compute expected outputs
    const vluint32_t e_resp = expected_resp(out, len, cmd_ready_latency);
    const vluint32_t e_cmd  = expected_cmd(out);

    // strobe command into the DUT
    mDut.len_bytes  = len;
    mDut.out_strobe = out;
    mDut.task_valid = 1;
    wait_cycles(1);
    mDut.task_valid = 0;

    // prep the output stream for potential command and wait
    // for response
    vluint32_t a_cmd = 0xFFFFFFFF;
    mDut.aso_cmd_ready = 0;

    // some for loop abuse to handle the latency counter
    for(uint32_t latency = 0; 0 == mDut.resp_valid; ++latency) {
        
        // delay asserting ready
        if(latency > cmd_ready_latency) {
            mDut.aso_cmd_ready = 1;
        }

        // capture the command data
        if (mDut.aso_cmd_ready && mDut.aso_cmd_valid) {
            a_cmd = mDut.aso_cmd_data;
        }

        wait_cycles(1);
    }

    // de-assert stream strobe
    mDut.aso_cmd_ready = 0;
    wait_cycles(1);

    // collect the actual response
    const vluint32_t a_resp = mDut.resp;
    
    // validate results
    if (e_resp != a_resp) {
        mFailCount++;
        std::cout << "-------------------------------------------------\n";
        std::cout << "Response failed\n";
        std::cout << "out = " << out << " len = " << len << " latency = " << cmd_ready_latency << '\n';
        std::cout << "Expected: " << e_resp << '\n';
        std::cout << "Actual  : " << a_resp << '\n';
        std::cout << "-------------------------------------------------\n";
    }

    if (e_resp == tb_out_cmd_task_icd_pkg::TASK_VALID && e_cmd != a_cmd) {
        mFailCount++;
        std::cout << "-------------------------------------------------\n";
        std::cout << "Command failed\n";
        std::cout << "out = " << out << " len = " << len << " latency = " << cmd_ready_latency << '\n';
        std::cout << "Expected: " << e_cmd << '\n';
        std::cout << "Actual  : " << a_cmd << '\n';
        std::cout << "-------------------------------------------------\n";
    }
}

void SimDut::exeErrorTest()
{
    std::cout << "Execution Error Test\n";
    reset();
    task(0, VALID_LEN, TIMEOUT); // valid task that timeout
}

void SimDut::invalidLenTest()
{
    std::cout << "Invalid Length Test\n";
    reset(); 
    task(0, VALID_LEN+1); // length too large
}

void SimDut::outputTest()
{
    constexpr vluint32_t start = 0;
    constexpr vluint32_t end   = 100000;
    std::cout << "Output Test [" << start << " - " << end << "]\n";
    reset();

    for (vluint32_t out = start; out <= end; ++out) {
        task(out, VALID_LEN); 
    }
}

void SimDut::reset(vluint64_t cycles)
{
  mDut.rst           = 1;
  mDut.task_valid    = 0;
  mDut.len_bytes     = 0;
  mDut.out_strobe    = 0;
  mDut.aso_cmd_ready = 0;
  wait_cycles(2);
  
  mDut.rst = 0;
  wait_cycles(1);
}

void SimDut::wait_cycles(vluint64_t cycles)
{
  const vluint64_t target_count = cycles + mPosEdgeCnt;

  while(target_count != mPosEdgeCnt) {
    tick();
  }
}

constexpr vluint32_t SimDut::expected_resp(vluint32_t out, vluint32_t len, uint32_t latency)
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

constexpr vluint32_t SimDut::expected_cmd(vluint32_t out)
{
    const uint32_t id = tb_out_cmd_cmd_icd_pkg::CMD_ID_OUT <<
        tb_out_cmd_cmd_icd_pkg::CMD_ID_LSB;

    const uint32_t o = out << tb_out_cmd_cmd_icd_pkg::OUT_CMD_LSB; 
    return id | o;
}

void SimDut::tick()
{ 
  mDut.clk ^= 1;
  mDut.eval();

  if (1 == mDut.clk) {
    ++mPosEdgeCnt;
  }

  mTrace.dump(mSimTime);
  mSimTime++;
}
