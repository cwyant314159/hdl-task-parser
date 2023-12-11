# HDL Task Parser

A small toy example of a control message parser written in synthesizable 
SystemVerilog.

## Running Simulations 

Each module in this project has a corresponding simulation in the __sim__
directory that exercises most (if not all) happy use cases as well as the 
various failure modes and edge cases. Simulations are implemented using the 
`cocotb` framework with `verilator` as the SystemVerilog backend. To faciliate
the install of cocotb, __sim/requirements.txt__ can be used to install all the 
necessary Python dependencies. Verilator can be installed from the system 
package manager or by referencing the official
[documentation](https://verilator.org/guide/latest/install.html). 

Simulation execution is controlled by a Makefile in the root of each simulation 
folder. These Makefile provide various targets that perform common simulation
tasks.

| Make Target | Purpose                                                  |
| ----------- | -------------------------------------------------------- |
| sim         | Run the simulation                                       |
| wave        | Run the simulation and open the dump.vcd file in GtkWave |
| lint        | Run the Verilator linter on the SystemVerilog source     |
| all         | Maps to the sim Make target                              |

It is recommended that the Python dependencies be installed in some kind of 
managed environment (e.g. `venv`). The script snippet below shows how to setup
a Python virtual environment and run a simulation.

```bash
cd sim 
python3 -m venv venv             # creates a folder named venv 
source venv/bin/activate         # activate the environment
pip3 install -r requirements.txt # install Python dependencies    
cd ./some-module                 # each module has their own folder
make lint                        # run lint on the HDL 
make                             # run the sim 
make wave                        # re-run the sim and open the wave file
```

## Basic Theory

The task parser in this project is a stripped down version of parser I used to 
work on at one of my previous jobs. Many of the reliability features have been 
removed, but the basic premise that structured packets of data are converted 
into a single command word used by internal logic is the same.

![basic-task-flow](./icd/img/basic-task-flow.png)

__TODO__

The HDL Task Parser is a simple parser that takes a structured message input 
based on some application specific ICD and outputs an application specific 
command word. 

The HDL Task Parser is a hardware implementation of a command and control
message parser in synthesizable SystemVerilog. The parser sinks a task from the
inbound stream, validates the header and payload, executes the task ID's
action, and sources a response on the outbound stream. Task execution
responsibility is forwarded to downstream hardware blocks using a single 32-bit
word called a command (cmd). Descriptions of task and command layouts are
described in the projects interface control document (icd).

