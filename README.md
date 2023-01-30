# HDL Task Parser

The HDL Task Parser is a hardware implementation of a command and control
message parser in synthesizable SystemVerilog. The parser inbound stream sinks
a task, validates the header and payload, executes the task ID's action, and
sources a response on the outbound stream.

## System Requirements

All HDL simulations are C++ based simulations that are built with the GCC
toolchain and Make. The SystemVerilog to C++ "conversion" is done using
[Verilator](https://www.veripool.org/verilator). All Makefiles and scripts
expect Verilator to be on the system path.

All simulations were developed and tested on MacOS and Linux. No attempt was
made to support Windows. Pull-requests to add Windows support are welcomed and
encouraged.

## Project Layout

- icd
- hdl
- sw
- sim
- sim/verilator support
- sim/common

## Running Software Implementation

<!-- TODO -->

## Simulation Makefiles

- run target will build verilate and run simulation
- lint target runs linter checks on SystemVerilog
- verilate target runs verilation and creates c++ class

<!-- Move the following sections to another markdown file -->
<!-- theory.md -->
## Task Parser "Theory"

## Software Implementation

## Hardware Implementation


