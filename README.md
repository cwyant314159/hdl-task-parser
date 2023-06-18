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
4. Python3
5. GtkWave

All HDL simulations are implemented using `cocotb`. A `requirements.txt` file
located in the `sim` folder contains all the necessary Python dependencies to
run the simulations. Each HDL module has a simulation under the `sim` directory
as well as an associated Makefile to support tasks such as linting, handling
VCD files for GtkWave, and running the simulation. See the section on
___Simulation Makefiles___ for more information on using the Makefile.

All simulations were developed and tested on MacOS and Linux. No attempt was
made to support Windows. Pull-requests to add Windows support are welcomed and
encouraged.

## Project Layout

### __`hdl`__

The SystemVerilog source for the task parser is located in the `hdl` folder at
the root of the project. For ease of simulation, each standalone block of the
task parser is given its own source file. A top level module that integrates
all the pieces of the task parser, is located in `task_parser.sv`.

### __`sim`__

A simulations for each HDL block is located in the `sim` folder. See the
section on ___Simulation Makefiles___ for more information on how to build and
run simulations.

### __`sw`__

A software implementation of the task parser is located in the `sw` folder. Not
only does the software task parser implement the same tasks as the HDL variant,
it also includes a framework for handling commands that are not suited for HDL
processing. Some tasks may have a variable length payload with an upperbound
that is too large to efficiently handle in hardware. Other tasks like resets
and status reporting typically require more system level information that is
often difficult to coordinate in hardware only implementations.

More details on the software implementation and its example application can be
found in the README of the `sw` folder.

## Simulation Makefiles

Each simulation for the HDL modules contains a Makefile that handles all the
specifics of linting and simulating that HDL block. The table below summarizes
the main Make targets of a simulation's Makefile.

| Target   | Purpose                                                  |
| -------- | -------------------------------------------------------- |
| sim      | Run the simulation                                       |
| wave     | Run the simulation and open the dump.vcd file in GtkWave |
| lint     | Run the Verilator linter on the SystemVerilog source     |
| all      | Maps to the sim Make target                              |

Simulations are implemented using `cocotb`. A `requirements.txt` file is
provided contains all the necessary dependencies to run the simulations.

Linting is performed by `verilator`. The Makefile assumes `verilator` is
available on the system PATH.

## Running Software Implementation

**TODO**

<!-- Move the following sections to another markdown file -->
<!-- theory.md -->
## Task Parser "Theory"

**TODO**

## Software Implementation

**TODO**

## Hardware Implementation

**TODO**