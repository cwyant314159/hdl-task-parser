# HDL Task Parser

The HDL Task Parser is a hardware implementation of a command and control
message parser in synthesizable SystemVerilog. The parser sinks a task from the
inbound stream, validates the header and payload, executes the task ID's
action, and sources a response on the outbound stream. Task execution
responsibility is forwarded to downstream hardware blocks using a single 32-bit
word called a command (cmd). Descriptions of task and command layouts are
described in the projects interface control document (icd).

## System Requirements

1. Make
2. GCC
3. Verilator

All HDL simulations are C++ based simulations that are built with the GCC
toolchain and Make. The SystemVerilog to C++ "conversion" is done using
[Verilator](https://www.veripool.org/verilator). All Makefiles and scripts
expect Verilator to be on the system path. Any sufficiently recent version of
Verilator should be good enough to run the simulations in this project.

All simulations were developed and tested on MacOS and Linux. No attempt was
made to support Windows. Pull-requests to add Windows support are welcomed and
encouraged.

## Project Layout

### __hdl__

The SystemVerilog source for the task parser is located in the __hdl__
directory at the root of the project. For ease of simulation, each standalone
block of the task parser is given its own source file. A top level module that
integrates all the pieces of the task parser, is located in
__task_st_parser.sv__ (as in task stream parser).

### __sim__

Simulations for the HDL blocks are located in the __sim__ directory. Each
module has its own simulation directory. See the section on
___Simulation Makefiles___ for more information on how to build and run
simulations. The __verilator_support__ folder in the simulation directory
contains a Makefile for compiling the Verilator supporting code located in
the Verilator install directory. The Makefile will compile the supporting code
into a static archive and copy all necessary headers to an include folder in
the support directory. See the
[verilator-ip-simulation-template](https://github.com/cwyant314159/verilator-ip-simulation-template)
for more information.

## Simulation Makefiles

Each simulation is contained in a folder under the `sim` directory. A Makefile
that is responsible for building, verilating, and running is located at the
root of each simulation folder. The table below summarizes the main Make
targets of a simulation's Makefile.

| Target   | Purpose                                              |
| -------- | ---------------------------------------------------- |
| lint     | Run the Verilator linter on the SystemVerilog source |
| verilate | Verilates HDL into C++                               |
| all      | Builds simulation. Verilates if necessary            |
| run      | Runs compiled simulation. Builds if necessary        |

When running a simulation, a start banner with the name of the simulation is
printed followed by the names of one or more test cases. Once the simulation is
finished, an end banner with the simulation is printed with test and failure
statistics

```bash
$ make run
=== OUTPUT COMMAND SIMULATION BEGIN ===
Output Test [0 - 100]
Invalid Length Test
Execution Error Test
=== OUTPUT COMMAND SIMULATION END ===
Tests: 103
Fails: 0
```

If a test fails during a simulation, an error message is printed with the
expected value and actual value as well as the simulation inputs that caused
the failure.

```bash
$ make run
=== OUTPUT COMMAND SIMULATION BEGIN ===
Output Test [0 - 100]
-------------------------------------------------
Response failed
out = 32 len = 20 latency = 0
Expected: 0
Actual  : 2
-------------------------------------------------
-------------------------------------------------
Command failed
out = 32 len = 20 latency = 0
Expected: 268435488
Actual  : 4294967295
-------------------------------------------------
Invalid Length Test
Execution Error Test
=== OUTPUT COMMAND SIMULATION END ===
Tests: 103
Fails: 2
```

## Running Software Implementation

<!-- TODO -->

<!-- Move the following sections to another markdown file -->
<!-- theory.md -->
## Task Parser "Theory"

<!-- TODO -->

## Software Implementation

<!-- TODO -->

## Hardware Implementation

<!-- TODO -->