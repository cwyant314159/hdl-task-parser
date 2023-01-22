#pragma once

#include <cstdint>
#include <verilated.h>

#include "tb_out_cmd_task_icd_pkg.h"

namespace Sim {

class OutTask {

public:
    static constexpr vluint32_t    OUT_MIN   = 0;
    static constexpr vluint32_t    OUT_MAX   = 31;
    static constexpr vluint32_t    VALID_LEN = tb_out_cmd_task_icd_pkg::HEADER_BYTES + 4;
    static constexpr std::uint32_t TIMEOUT   = 100000;

    OutTask(vluint32_t out = OUT_MIN, vluint32_t len = VALID_LEN,
            std::uint32_t cmd_ready_latency = 0) noexcept;
    vluint32_t GetResp() const noexcept;
    vluint32_t GetCmd() const noexcept;

    vluint32_t    out;
    vluint32_t    len;
    std::uint32_t latency;

private:
    vluint32_t resp;
    vluint32_t cmd;
};

} // namespace Sim
