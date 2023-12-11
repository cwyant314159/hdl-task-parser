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

At a previous job, I spent a lot of time writing embedded applications that
were responsible for acting and responding to tasks over Ethernet connections.
The implementation usually involved defining structs that represented task 
messages that would be processed into whatever necessary data structure needed 
by the task's action. Most actions would need to be carried out by custom IP 
written internally. To make life easier on the IP designers, their data inputs 
computed from task messages would be a single command word of some predefined 
width. The width would be determined ahead of the finalized design and be large
enough to handle all possible tasks for the application. For the purposes of 
this project, command words can be assumed to be 32-bits wide.

![basic-task-flow](./icd/img/basic-task-flow.png)

For 99.99% of the tasks, a software implementation was fast enough to meet 
application requirements. The latency caused by the Ethernet LAN stack and 
other background processing on the processor core running the task parser were
negligble compared to the custom IP (think relay switching speeds). However, in 
the 0.01% of tasks where latency was required to be as minimal as possible, a 
faster/parallel HDL solution was needed. As stated earlier tasks were generally
sent over Ethernet, and the FPGA vendor's Ethernet MAC IP just happened to use 
streaming interfaces as the main mechanism for moving packets. This allowed 
shim logic to be placed between the Ethernet MAC and the processor running the 
software task parser. The shim was a "large" logic block that was responsible 
for scanning packets from the Ethernet MAC and determining if the packet was 
one of the application tasks. If the packet was an application task and if that
particular task type was flagged for hardware processing, the shim would 
redirect that packet to a logic block that could decode and execute that task:
the HDL task parser.

![hdl-task-flow](./icd/img/hdl-task-flow.png)











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

