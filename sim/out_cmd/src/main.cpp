#include "out_cmd_dut.hpp"

// MAIN
int main(int argc, char** argv)
{
    Verilated::commandArgs(argc, argv);

    Sim::OutCmdDut simDut("out_cmd");

    simDut.OutputTest();
    simDut.InvalidLenTest();
    simDut.ExeErrorTest();

    return simDut.GetFailCount() > 0;
}
