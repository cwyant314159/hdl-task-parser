#pragma once

#include <cstdint>
#include <string>

#include <verilated_vcd_c.h>

namespace Sim {

template<typename TDut>
class Dut {

public:
    Dut(std::string wave_file)
    {
        wave_file.append(".vcd");
        Verilated::traceEverOn(true);
        this->mDut.trace(&this->mTrace, 5);
        this->mTrace.open(wave_file.c_str());
    }

    ~Dut()
    {
        this->mDut.final();
        this->mTrace.close();
    }
    
    std::uint64_t GetFailCount()
    {
        return this->mFailCount;
    }

    void WaitCycles(std::uint64_t cycles)
    {
        const uint64_t target_count = cycles + this->mPosEdgeCnt;

        while (target_count != mPosEdgeCnt) {
            this->tick();
        }
    }

protected:
    virtual std::uint8_t toggle_clk() = 0;

    TDut mDut;
    std::uint64_t mTestCount  = 0;
    std::uint64_t mFailCount  = 0;

private:
    void tick()
    {
        const std::uint8_t clk_state = this->toggle_clk();
        this->mDut.eval();

        if (1 == clk_state) {
            ++this->mPosEdgeCnt;
        }

        this->mTrace.dump(this->mSimTime);
        ++this->mSimTime;
    }

    VerilatedVcdC mTrace;
    std::uint64_t mSimTime    = 0;
    std::uint64_t mPosEdgeCnt = 0;
};

} // namespace Sim
