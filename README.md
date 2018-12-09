# CS520
Architecture lecture in Binghamton University with Prof. Ghose

honestly, I am a little behind the class process

# class note study
cann't follow the lecture, what a sad story

## The APEX Pipeline: Details Page.32

## The APEX Pipeline - some noteworthy Features, Page.36

## The APEX pipeline - some Timing issues, Page.38

## More on Basics of Instruction Pipeline, Page.39

## efficiency issues, page.40

## CPI - ideal vs. Real, page.41

## depicting usage of pipeline stages, page.43

## a more realistic instruction pipleline - multiple execution units, page.44

## function pipelines: pipelines for implementing operation, page.45

## pipeline classifications, page.49

## number of pipeline stage: what's optimum?, page.50

## de-couple pipelines: making pipeline sections independent of each other, page.51
TODO: ski

## a fundamental result, page.52
TODO: ?? don't understand

## the impact of pipeline bottlenecks, page.53

## avoid pipeline bottlenecks: replication vs. segmentation, page.54
TODO: ski

## improve the thruput of uni-function, Non-linear function pipelines, page.55

## multifunction pipelines, page.58

# part 2, instruction pipelines, page.61

## instruction pipelins: scalar pipelines and some terminology, page.62

## recap: the APEX pipeline - some noteworthy features, page.64

## Enhancing Machine Parallelism: Multiple Function Units, page.66

## enhancing machine parallelism: pipelined function units, page.71

## enhancing machine parallelism: issue queue, page.73

## terminology related to instruction scheduling, page.79

## instruction wait states in out-of-order machines, page.80
TODO: ski

## data dependencies,page.81

## detecting data dependencies,page.88

## data flow graphs, page.92
TODO: what's the TSF and R1 data dependence there??

## coping with data dependencies, page.93
table of content
hardware techniques
    Simple interlocking for in-order machines
    Data forwarding
    Dynamic Instruction Scheduling
    - register renaming (most prevalent)
    - CDC 6600 Scoreboarding (historical)
    - Tomasulo's "algorithm" (historical - similarities with register renaming)
    Decoupled execute - acccess mechanism
    multithreading
software techniques
    software interlocking
    software pipelining

## simple interlocking for pipelines with in-order issuing mechanism, page.94
TODO: picture, status bit array(2 read ports), should be 3 read ports??
TODO: condition b ensures anti- and output- dependence ?? how does it work??
TODO: how handles mutiple FUs and out-of-order writes back ??

## data forwarding, page.96

## dynamic instruction scheduling, page.100

## terminology related to Dynamic Instruction scheduling, page.101

## register renaming: preamble, page.102

## register renaming, page.103

## register renaming adapted for APEX, page.106

don't understand the graph, why two output?: one for data, one for address

TODO: why make Decode/rename1 and rename2/dispatch ??

what's the difference between register renaming and Tomasulo's algorithm, page.116?:
Tomasulo's algorithm is one of mechinism to implement data forwarding

## Forwarding register value using the register address as a "tag", page.117

## alterenative implementation of register renaming, page.118
Uses centralized Issue Queue (IQ) and associative tag matching
Most modern pipelined processors use renaming and associative tag matching with a centralized IQ or distributed IQs (reservation stations).

## handling Dependencies over condition codes in Register Renaming, page.123

## obtaining operands in a Dynamically Scheduled processor: choices, page.124
TODO: not really understand

## the CDC 6600 Scoreboarding Mechanism, page.125
page.127, why not include status of destination register?: because destination register is busy when has a instruction in pipeline
because instruction doesn't dispatch when source registers are busy
    dispatch rule: output dependences
    start up rule: flow dependences, page.128
    completion rule: anti-dependences, page.129
example of CDC 6600 scoreboard mechanism, page.131
potential improvement, page.134
TODO: Allow operands to be transferred to FUs in a piecemeal fashion?? how to improve??


## Tomasulo's algorithm, page.135
CDB: common data bus
source tags, defined in page.138
### forwarding using the source tag, page.141
### instruction dispatching rule, page.142
### handling VFU completions, page.144

TODO: draw the Gantt chart, page.150 ??

## handling Dependencies over condition Code in Tomasulo's Technique, page.152

## the software interlocking algorithm(continued), page.154
I5 and I6 is output dependence, page.159 ?: STORE <src1> <src2> literal

## software pipelining: another software interlocking algorithm, page. 162
TODO: to be continue, page.164

## using multithreading to avoid interlocking delays, page. 168

## back to Dynamic scheduling: the LOAD/STORE Queue, page.170
TODO: last point, why forward value to be stored, why not address of memory??

## Dispatch and Issue components for a centralized IQ, page.173

## handling interurpts in out-of-order execution pipelines, page.174

## interrupts in pipelined CPUs: the problem, page.175

## interrupts in pipelined CPUs: some terminology, page.177

## handling interrupts in pipelined - overivew, page.178

## Recorder Buffers, page.181

## Aside: implementing the ROB, page.193

## constructing the precise state in software, page.194

## real machines: implementations of register renaming, page.197

techniques for Avoiding or reducing the penalty of Braching, page.222
--------------------------------------------------------------------------------
table of content

- Branch Handling: Early compares, page.223
- Unconditionallly Fetching Instruction Stream Starting at the target
- static Prediction
- Delayed Braching and Delayed Branching with Squashing
- Dynamic Branch Prediction:
    + Branch History Table
    + Branch Target Buffers
    + ALternate Stream Prefetching Based on Prediction
- Separate Branch Unit
- Predicated Execution(aka conditional assignments/guarded execution)
- Branch Folding
- Tow-level Branch Prediction
- Hybrid Branch Handling Techniques & Others

Speculative Execution, page.266
--------------------------------------------------------------------------------

PART IV: Memory System
================================================================================

techniques for Improving Memory Performance, page.288
--------------------------------------------------------------------------------
- Memory interleaving
- Cache memory
- Newer memory interfaces
- prefetching
- Stream buffers
- Store bypassing by loads and predicting store bypasing("dynamic memory disambiguation")
- Cache miss prediction
- Simulataneous multithreading

### RAM Devices, page.289
more about hardware working principle

### DRAM Technologies, page.303

### Memory Interleaving, page.309

### Early DRAM memory System, page.318

### Modern Memory System Based on DDRs, page.319

### Cache Memory Systems, page.328

#### Cache Replacement Algorithms, page.378

### Prefetching in Contemporary Processors, page.400

PART VI: Multicore Microprocessors and Parallel Systems, page.445
================================================================================



### The abstraction of Shared Memory in Shared Memory MPs, page.471

### the sequential Consitency Model: Details, page.474

### Other Cache coherence protocols and implementation, page.488

PART VII: VILW and EPIC Approaches to Exploiting LIP, page.502
================================================================================


I will get ride of printed version any way, hard to read without marks
# CS520 Arch
password: KOoLCHiPs!18
I like this style

page. 29
master-slaver latch

sign-extension: 10bits to 32 bits, fill the extended bits with sign bit value

question to ask
- SSE interview

things to review for exam 2
--------------------------------------------------------------------------------
- Low order interleaving in a memory system
- Row buffer hits in a DRAM memory system
- increasing the cache line size will
- Hardware-initiated prefetching
- TSO model of memory consistency
- MESI cache coherence protocol
- Spin locks
- Caching of lock variables in a shared memory multiprocessor

x Tormasolu CDC6600 scoreboarding
x details of Branch predictor
x DRAM Details
x interconnection Networks

DONE: renaming
O software interlock
DONE: forwarding
DONE: precise state on interrupts
O mem heriati
O different cache designs and trade offs
O prefetching
O Flynn's taxonomy

15 T/F question
question II
question III
