#include "dut.hpp"

// MAIN
int main(int argc, char** argv)
{
    Verilated::commandArgs(argc, argv);

    Sim::Dut simDut;

    simDut.OutputTest();
    simDut.InvalidLenTest();
    simDut.ExeErrorTest();

    return simDut.GetFailCount() > 0;
}
