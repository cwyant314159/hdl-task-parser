#include "out_task.hpp"

#include "tb_out_cmd_cmd_icd_pkg.h"

namespace Sim {

OutTask::OutTask(vluint32_t out, vluint32_t len, std::uint32_t cmd_ready_latency) noexcept
    : out(out), len(len), latency(cmd_ready_latency)
{
    if (this->len != VALID_LEN) { 
        this->resp = tb_out_cmd_task_icd_pkg::HEADER_INVALID;
    } else if (this->out > OUT_MAX) {
        this->resp = tb_out_cmd_task_icd_pkg::PAYLOAD_INVALID;
    } else if (this->latency > TIMEOUT) { 
        this->resp = tb_out_cmd_task_icd_pkg::EXE_ERROR;
    } else {
        this->resp = tb_out_cmd_task_icd_pkg::TASK_VALID;
    }

    if (tb_out_cmd_task_icd_pkg::TASK_VALID == this->resp) {
        
        constexpr uint32_t id = tb_out_cmd_cmd_icd_pkg::CMD_ID_OUT <<
            tb_out_cmd_cmd_icd_pkg::CMD_ID_LSB;

        const uint32_t o = this->out << tb_out_cmd_cmd_icd_pkg::OUT_CMD_LSB; 
    
        this->cmd = id | o;

    } else {
        this->cmd = 0xFFFFFFFF;
    }
}

vluint32_t OutTask::GetResp() const noexcept
{
    return resp;
}

vluint32_t OutTask::GetCmd() const noexcept
{
    return cmd;
}

} // namespace Sim
