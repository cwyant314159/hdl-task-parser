#include "sim_dut.hpp"

// MAIN
int main(int argc, char** argv)
{
    Verilated::commandArgs(argc, argv);

    SimDut simDut;

    simDut.reset();
    simDut.outputTest();
    simDut.invalidLenTest();
    simDut.exeErrorTest();

    return simDut.get_fail_count() > 0;
}
