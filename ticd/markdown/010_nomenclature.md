Nomenclature {#nomenclature}
============================

Like many technical documents, the TICD API document uses jargon and domain
specific terms to efficiently convey ideas and concepts. This seciton briefly
describes some key terms used throughout this API document.

Application {#application}
--------------------------

When viewing a system at a high level, the term application is used to refer to
a set of controllers and devices interacting with one another to perform some
task. If discussing a device or controllercontroller, the term application
usually refers to the actual compiled software running on a processor.

Device {#device}
----------------

The TICD framework describes a device as an entity that is capable of processing
inbound tasks and producing outbound status responses to thoses tasks. A device
is not able to generate tasks.

Controller {#controller}
------------------------

A controller is similar to a device with the exception that a controller is
able to generate tasks. Controllers are not required to respond to inbound
tasks and may be used as task sources only.

Task {#task}
------------

A task is a packed structure (@ref TicdTask) that contains the TICD header and
an optional payload. Tasks will typically instruct a device or controller to do
an action or produce a response. Inbound messages to a device will always be a
task while the same is not always true for a controller.

Response {#response}
--------------------

A response is a packed structure (@ref TicdTask) that contains a TICD header and
an optional payload. All task in a TICD application are acknowledged using a
response. Most responses are simply a header with the status field set to an
appropriate status code, but the optional payload of a response can be filled
with arbitrary data specified during application design.

TICD Message {#ticd_message}
----------------------------

A TICD message is a structure (@ref TicdMsg) that contains a task (or response)
and a meta buffer. The only purpose of this structure is to associate a
particular task or response with a meta buffer.  The TICD framework does not do
anything with this meta buffer other than copy it from the task message to the
response message.

The meta buffer is an array of bytes that the application can used to pass
trasmission details (file descriptors, UART number, or sender's IP address and
port) from the receive logic to the transmit logic. See the section 
[Message Meta Buffer](@ref message_meta_buffer) for more detail.

TICD Header {#ticd_header}
--------------------------

A TICD header is a packed structure (@ref TicdHeader) that contains the details
necessary for the framework to dispatch tasks to the appropriate task handler.
The following diagram shows the layout of the TICD header:

<table>
  <tr>
    <th>31</th>
    <th>30</th>
    <th>29</th>
    <th>28</th>
    <th>27</th>
    <th>26</th>
    <th>25</th>
    <th>24</th>
    <th>23</th>
    <th>22</th>
    <th>21</th>
    <th>20</th>
    <th>19</th>
    <th>18</th>
    <th>17</th>
    <th>16</th>
    <th>15</th>
    <th>14</th>
    <th>13</th>
    <th>12</th>
    <th>11</th>
    <th>10</th>
    <th>9</th>
    <th>8</th>
    <th>7</th>
    <th>6</th>
    <th>5</th>
    <th>4</th>
    <th>3</th>
    <th>2</th>
    <th>1</th>
    <th>0</th>
  </tr>
  <tr>
    <td colspan=32 align=center>Length</td>
  </tr>
  <tr>
    <td colspan=16 align=center>Sequence ID</td>
    <td colspan=8  align=center>Sub-ID</td>
    <td colspan=8  align=center>ID</td>
  </tr>
  <tr>
    <td colspan=32 align=center>Status</td>
  </tr>
</table>

The length field contains the number bytes in a task (including the header).

A sequence ID is typically used a monotonic increasing count to allow for
for task/response tracing in a system. This use is not enforced by the
framework. System designers are free to use this field as needed. The framework
simply copies this field from the inbound task to the outbound response.

ID and sub-ID are used as the main mechanism for dispatching tasks to their
appropriate handler. The framework defines a set of standard task ID's defined
in @ref ticd_standard_task_id. The Sub-ID fields are only used in the processing
of the standard task ID's. No other Sub-ID parsing is done within the framework.
A system designer can use this field as they see fit in the application specific
tasks. It is recommended to not deviate from the framework's usage of Sub-ID for
the standard tasks.

The status field of the header is used by responses to convey the success or
failure mode of a particular task. Although present in both tasks and responses,
this field typically only has meaning in a response. The framework defines a
standard set of status codes in @ref ticd_standard_status_code. System designers
are free to add their own set of status codes to suit their application needs.

Error Code {#error_code}
------------------------

Not to be confused with status codes, error codes (@ref TicdErr) are software
constructs used by the framework's API to convey error handling information of a
particular API function call.

Handler {#handler}
------------------

Application writers add custom task ID handling through the use of handler
functions that are passed to the framework through @ref ticd_task_table_entry
structures. Entry structures contain function pointers with the addresses of the
application writer's custom handlers. See
[User Defined Tasks](@ref user_defined_task) for more information on creating
and using custom ID handlers.

Glue Function {#glue_function}
------------------------------

Framework glue functions allow application developers to extend portions of the
TICD framework to better suit their application needs. Most of the glue
functions are opt-in as in they have weak definitions in the framework. The
exceptions to the opt-in strategy are @ref ticd_glue_rx_message and
@ref ticd_glue_tx_message. Application developers are required to define these
functions. Failure to define these functions will result in a linker error.

Ready Gate {#ready_gate}
------------------------

Many devices are only able to process task messages after reaching a certain
operational state. To prevent unwanted task processing, the framework has the
concept of a ready gate. The ready gate is simply a glue function
(@ref ticd_glue_application_ready) that returns @ref TICD_SUCCESS if the
application ready and @ref TICD_FAILURE otherwise. Tasks are rejected with a
@ref TICD_STATUS_NOT_READY status code in the response when the application is 
not in an operational state. Some task IDs are allowed to bypass this gating
such as the @ref TICD_TASK_RESET or @ref TICD_TASK_BOOT_STATUS. Application
writers can define application specific ID's that can bypass the ready gate by
returning @ref TICD_SUCCESS from @ref ticd_glue_task_can_bypass_ready.

Initialization Data {#initialization_data}
------------------------------------------

Some applications require initialization data before transitioning to a ready
state (i.e. passing tasks through the [Ready Gate](@ref ready_gate)). To allow
data to be received via a task, @ref TICD_TASK_INITIALIZATION_DATA is allocated
as apart of @ref ticd_standard_task_id and is the official task ID for giving
applications initialization data using TICD tasks. This task ID is not a
mandatory, and applications are free to provide initialization data through
other side channels if required. There is no default implementation for this
task ID since initialization data is application specific.
