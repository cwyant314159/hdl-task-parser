/**
 * @file   bank_cmd_dut.hpp
 * @brief  Concrete Bank Command hardware block DUT
 * @author Cole Wyant
 * @date   2023-01-27
 */
#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

#include <verilated.h>

#include "dut.hpp"
#include "tb_bank_cmd.h"
#include "tb_bank_cmd_task_icd_pkg.h"

/**
 * Components related to HDL simulations are contained in the @c Sim namespace.
 */
namespace Sim {

/**
 * @class BankCmdDut
 * @brief Concreate Bank Command DUT
 */
class BankCmdDut: public Dut<tb_bank_cmd> {

public:

    /**
     * @brief Bank Command DUT constructor
     *
     * @param wave_file The name of the simulation's VCD wave file
     *
     * @note The wave file name given as a parameter to this constructor is
     * assumed to not have the file extension. The file extension is appended
     * to the pased in filename.
     */
    BankCmdDut(std::string wave_file);

    /**
     * @brief Bank Command DUT destructor
     */
    ~BankCmdDut();

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
     * @brief An alias for a tuple that contains a bank enable and bank value
     */
    using BankEnVal = std::tuple<vluint32_t, vluint32_t>;
    
    /**
     * @brief An alias for a container of bank enable and value tuples
     */
    using BankEnValContainer = std::vector<BankVal>;

    /**
     * @brief An alias for a container of generated commands
     */
    using CmdContainer = std::vector<vluint32_t>

    /**
     * @brief Bank index in the bank and bank value tuple
     */
    static constexpr std::size_t BANK_IDX = 0;

    /**
     * @brief Value index in the bank and bank value tuple
     */
    static constexpr std::size_t VAL_IDX = 0;

    /**
     * @brief Number of bytes per bank value pair
     */
    static constexpr vluint32_t BANK_VAL_PAIR_BYTES = 4 * 2; // four bytes * two words 

    /**
     * @brief Minimum valid bank enable value
     */
    static constexpr vluint32_t ENABLE_MIN = 0;

    /**
     * @brief Maximum valid bank enable value
     */
    static constexpr vluint32_t ENABLE_MAX = 15;

    /**
     * @brief Minimum valid bank value
     */
    static constexpr vluint32_t VAL_MIN = 0;
    
    /**
     * @brief Maximum valid bank value
     */
    static constexpr vluint32_t VAL_MAX = 255;
    
    /**
     * @brief Execution error timeout cycle count
     */
    static constexpr std::uint32_t TIMEOUT = 100000;

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
     * @param bev_pairs container of bank enable and bank value pairs
     * @param latency Command stream ready assertion latency cycles.
     */
    void act(const BankEnValContainer& bev_pairs, std::uint32_t latency = 0);

    /**
     * @brief Compute task response from task fields
     * @param bev_pairs container of bank enable and bank value pairs
     * @param latency Command stream ready assertion latency cycles
     * @return expected status response of the DUT
     */
    vluint32_t get_resp(const BankEnValContainer& bev_pairs, std::uint32_t latency) const noexcept;

    /**
     * @brief Compute Output Command from task fields
     * @note An invalid task will return an empty conatiner
     * @param bev_pairs container of bank enable and bank value pairs
     * @param latency Command stream ready assertion latency cycles
     * @return a container of expected generated commands from the DUT
     */
    CmdContainer get_cmd(const BankEnValContainer& bev_pairs, std::uint32_t latency) const noexcept;
    
    /**
     * @brief Set the inputs of the DUT
     *
     * This helper function sets the bank, value, and length inputs of the DUT
     * from a @c BankValContainer. The valid strobe is not asserted in this
     * function.
     *
     * @param bev_pairs container of bank enable and bank value pairs
     */
    void set_inputs(const BankenValContainer& bev_pairs);

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
