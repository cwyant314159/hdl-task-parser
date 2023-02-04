/**
 * @file   dut.hpp
 * @brief  Generic HDL device under test interface
 * @author Cole Wyant
 * @date   2023-01-21
 */
#pragma once

#include <cstdint>
#include <string>

#include <verilated_vcd_c.h>

/**
 * Components related to HDL simulations are contained in the @c Sim namespace.
 */
namespace Sim {

/**
 * @class Dut
 * @brief An abstract HDL device under test
 * @tparam TDut The class generated by verilator for the DUT
 */
template<typename TDut>
class Dut {

public:

    /**
     * @brief Abstract DUT constructor
     *
     * The constructor handles the initialization of the verilator DUT object
     * with tracing as well as opening the VCD wave file for trace recording.
     *
     * @param wave_file The name of the simulation's VCD wave file
     *
     * @note The wave file name given as a parameter to this constructor is
     * assumed to not have the file extension. The file extension is appended
     * to the pased in filename.
     */
    Dut(std::string wave_file)
    {
        wave_file.append(".vcd");
        Verilated::traceEverOn(true);
        this->mDut.trace(&this->mTrace, 5);
        this->mTrace.open(wave_file.c_str());
    }

    /**
     * @brief Abstract DUT destrutor
     *
     * The destructor handles finalizing the verilator DUT object and closing
     * the VCD wave file.
     */
    ~Dut()
    {
        this->mDut.final();
        this->mTrace.close();
    }
    
    /**
     * @brief Get the current number of failed DUT test cases
     * @return The number of failures
     */
    std::uint64_t GetFailCount()
    {
        return this->mFailCount;
    }

    /**
     * @brief Wait for the given number of simulation cycles to pass
     *
     * Cycles are defined as the time between rising edges of a clock (or
     * clock-ish) signal. This method only keeps track of the elapsed positive
     * edges in the simulation and returns once the target number of edges has
     * passed. The low-level mechanics of a time passage is handled by the
     * internal @c tick method.
     *
     * @param cycles Number of cycles to wait
     */
    void WaitCycles(std::uint64_t cycles)
    {
        const uint64_t target_count = cycles + this->mPosEdgeCnt;

        while (target_count != mPosEdgeCnt) {
            this->tick();
        }
    }

protected:

    /**
     * @brief Pure virtual DUT clock toggle
     *
     * This method must be implemented in derived classes to expose the DUT 
     * specific clock toggling. This method allows the @c tick method to be
     * private to this abstract class. Derived DUT classes do not need to
     * handle time passage at all except calling @c WaitCycles when necessary.
     *
     * @return The state of the clock after the toggle
     *
     * @note If a DUT is purely combinational (i.e. does not have a clk), the
     * derived DUT should simple return a 1 from this method.
     */
    virtual std::uint8_t toggle_clk() = 0;

    /**
     * @brief Verilator DUT object
     */
    TDut mDut;

    /**
     * @brief Total number of DUT tests executed
     */
    std::uint64_t mTestCount  = 0;

    /**
     * @brief Total number of DUT test failures
     */
    std::uint64_t mFailCount  = 0;

private:

    /**
     * @brief Internal DUT tick
     *
     * This method handles the passage of time (i.e. clock toggling). Each call
     * to this method represents a clock half cycle's worth of time passing.
     * Verilator DUT object eval() calls and VCD file recording also is handled
     * by this function.
     */
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

    /**
     * @brief Verilator tract object
     */
    VerilatedVcdC mTrace;

    /**
     * @brief Simulation time marker (VCD wave file x-axis)
     */
    std::uint64_t mSimTime    = 0;

    /**
     * @brief Number of elapsed clock positive edges
     */
    std::uint64_t mPosEdgeCnt = 0;
};

} // namespace Sim