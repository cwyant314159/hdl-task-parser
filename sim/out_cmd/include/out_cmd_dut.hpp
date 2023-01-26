/**
 * @file   out_cmd_dut.hpp
 * @brief  Concrete Output Command hardware block DUT
 * @author Cole Wyant
 * @date   2023-01-21
 */
#pragma once

#include <cstdint>

#include <verilated.h>

#include "dut.hpp"
#include "tb_out_cmd.h"
#include "tb_out_cmd_task_icd_pkg.h"

/**
 * Components related to HDL simulations are contained in the @c Sim namespace.
 */
namespace Sim {

/**
 * @class OutCmdDut
 * @brief Concreate Output Command DUT
 */
class OutCmdDut: public Dut<tb_out_cmd> {

public:

    /**
     * @brief Output Command DUT constructor
     *
     * @param wave_file The name of the simulation's VCD wave file
     *
     * @note The wave file name given as a parameter to this constructor is
     * assumed to not have the file extension. The file extension is appended
     * to the pased in filename.
     */
    OutCmdDut(std::string wave_file);

    /**
     * @brief Output Command DUT destructor
     */
    ~OutCmdDut();

    /**
     * @brief Execution Error Test
     *
     * This test exercises the failed execution scenario. A failed execution
     * results from the output command stream not being ready for a constant
     * defined number of clock cycles.
     */
    void ExeErrorTest();

    /**
     * @brief Invalid Header Length Test
     *
     * This test exercises the invalid header length scenario.
     */
    void InvalidLenTest();

    /**
     * @brief Output Task Test
     *
     * This test exercises both valid and invalid output task scenarios. The
     * test will cycle through all possible valid output tasks as well as
     * several invalid (with respect to payload) output tasks.
     */
    void OutputTest();

private:

    /**
     * @brief Valid length of an output task
     */
    static constexpr vluint32_t    VALID_LEN = tb_out_cmd_task_icd_pkg::HEADER_BYTES + 4;
    
    /**
     * @brief Minimum valid output value for an output task
     */
    static constexpr vluint32_t    OUT_MIN   = 0;
    
    /**
     * @brief Maximum valid output value for an output task
     */
    static constexpr vluint32_t    OUT_MAX   = 31;
    
    /**
     * @brief Execution error timeout cycle count
     */
    static constexpr std::uint32_t TIMEOUT   = 100000;

    /**
     * @brief Execute a payload in the DUT
     *
     * This method will strobe in the given task and capture the resultant
     * response code and potential command. The command stream's ready strobe
     * can be delayed a certain number of clock cycles by setting the latency 
     * parameter to a non-zero value. Both the response code and command are
     * validated. Any errors are displayed and recorded in the base class
     * members.
     *
     * @param out Output task output field
     * @param len Output task header length field
     * @param latency Command stream ready assertion latency cycles.
     */
    void act(vluint32_t out, vluint32_t len = VALID_LEN, std::uint32_t latency = 0);

    /**
     * @brief Compute task response from task fields
     * @param out Output task output field
     * @param len Output task header length field
     * @param latency Command stream ready assertion latency cycles
     */
    constexpr vluint32_t get_resp(vluint32_t out, vluint32_t len, std::uint32_t latency) const noexcept;

    /**
     * @brief Compute Output Command from task fields
     * @note An invalid task will return a command of all 1's
     * @param out Output task output field
     * @param len Output task header length field
     * @param latency Command stream ready assertion latency cycles
     */
    constexpr vluint32_t get_cmd(vluint32_t out, vluint32_t len, std::uint32_t latency) const noexcept;
    
    /**
     * @brief Assert the DUT's reset input for a given number of cycles
     * @param cycles Number of cycles to assert the reset input
     */
    void reset(std::uint64_t cycles = 2);
    
    /**
     * @brief Toggle the DUT's clock input
     */
    std::uint8_t toggle_clk();
};

} // namespace Sim
