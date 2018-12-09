it's gonna be a big project, go get it

### data structure
- URF: R0~R39, unified register file
    extension to hold the Z flag
    one-bit status flag that indicates if their contents are valid
    the free bit the indicates if the entry is free
- ARF: R0~R15
    PRF: ?: using RAT and R_RAT, ARF is just in our mind
- RAT: register alias table
- back-RAT: back-end register alias table ?: point to commited value in URF

- IQ: 16 entries
    field to hold the cycle count at the time of dispatch
- ROB: 32 entries
- LSQ: 20 entries

tag: URF index
stage: src1_valid src1_tag

broadcast_tag
broadcast_data


### order of excute
- fetch
- drd
- IQ
- intFU
- mulFU
- LSQ
- mem
- ROB

stage():
  stage->stalled = UNSTALLED
  if stage->busy== BUSY_NEW
    stage->busy = STAGE_DELAY

  if stage->busy > BUSY_DONE
    work on stage data;
    stage->busy--;

  if stage->busy==DONE
    if copytoNextStage() == -1
      stage->stalled = STALLED
    else
      stage->busy = BUSY_WAIT

copytoNextStage():
  if nextStage->busy > BUSY_DONE || nextStage->stalled == STALLED
    return -1;
  else
    do copy;
    nextStage->busy = BUSY_NEW
    return 0;

### ISA:
- [ ] MOVC RD #NUM
// - [ ] MOV RD R1

- [ ] ADD RD R1 R2
- [ ] SUB RD R1 R2
- [ ] MUL RD R1 R2
// - [ ] DIV RD R1 R2
- [ ] ADDL RD R1 #NUM
- [ ] SUBL RD R1 #NUM

- [ ] AND RD R1 R2
- [ ] OR RD R1 R2
// - [ ] NOT RD R1
- [ ] EX-OR RD R1 R2

- [ ] LOAD RD R2 #NUM
- [ ] STORE R1 R2 #NUM

- [ ] BZ, #-12
- [ ] BNZ, #12
- [ ] JUMP, RS1, #12 /* absolute addr */
- [ ] JAL RD R1 #NUM
        RET implement as JUMP RD #0 /* RD saves the next addr of JAL */
- [ ] HALT

// may not be a good idea when it comes to out of order excution
// - [ ] PRINT_REG R1
// - [ ] PRINT_MEM R1 #NUM
- [ ] NOP

### questions
#### where renaming actually happen, D/RF or IQ ?:
  DRD stage, decode/renaming/dispatch
#### what data Struct RAT and R_RAT points to ?:
  RAT[R1] = UX, R_RAT[R1] = UY, after RAT[R1] is update, save old value in R_RAT[R1], used for recovering
#### how file_parse.c functions become useable in cpu.c, without include ?:
    in the original code, public file_parser function is declared in cpu.h, it's like one .h file and two implement .c file
#### don't need write back stage ?: do wirte back operations in the end of intFU, mulFU, memFU
How to do writeback operation?
At the end of circle T,
  - intFU finish calculating of "ADD R1(U1), R2(U2), R3(U3)". Then it copies result to broadcast_data, and associated ROB entry, and set the ROB entry completed
  - ROB check entry at the head, if it's completed, set R_RAT[R1]=U1 and retire the instrn by moving head pointer forward

#### TODO: STORE R1 R2 #3,
      when waiting in IQ, it doesn't need R1 ready, only need R2 ready
      when waiting in LSQ, it check addr_valid, and could wait for R1 ready

#### TODO_2: how does it organize spaces
this code is aim to wanna access a entry quickly, and queue it in any order
```cpp
  my_Class array[NUM];
  std::deque<my_Class&> q;
  q.push_back(array[0]);
```
a better way
```cpp
  my_Class array[NUM];
  std::deque<int> q; // q store the array_index
  q.push_back(0);
```

#### stage execution order
    ROB_run(cpu);   // commit instrns

    MEM_run(cpu);   // =>ROB, =>broadcast
    intFU_run(cpu); // =>ROB, =>broadcast or =>LSQ
    mulFU_run(cpu); // =>ROB, =>broadcast

    LSQ_run(cpu);   // fetch value from RAT or broadcast =>MEM
    IQ_run(cpu);    // fetch value from RAT or broadcast =>intFU or =>mulFU

    DRD_run(cpu);   // =>ROB, =>IQ or =>LSQ
    Fetch_run(cpu); // =>DRD



### some trick about vim
<C-w> _     - maximum current window
<C-w> =     - make windows all equal size

### others
I am facing a design choice here:
cpu.c contains the private data
cpu_helper.c wanna use private data to implement some helper functions(some lower level basic operations)
solution1: put private data into cpu.h and cpu_helper include it, but cpu.h is public to outside world of cpu module
solution2: move print/debug and basic functions from cpu_helper.c into cpu.c, making cpu_helper.c don't need to access cpu private data, but it make cpu.c grows too fast, like over 3600 lines
solution3: creating cpu_base.h, puting cpu module-wise public data and functions in their. Then cpu.c and cpu_helper include cpu_base.h, the could use module-wise public data

I like solution3, cpu module is divide into three sub-modules
- cpu_base: provide cpu basic data structure, is cpu module-wise public
- cpu_helper: include cpu_base.h, implement basic functions
- cpu: include cpu_base.h and cpu_helper.h, implement high level cpu logic, and public to outside world
- main: drive module, include cpu.h

- global: hold inter-module data and config data
- file_parse: using cpu_base.h data and public to cpu.c

with respect of variable naming rule, don't use RAT, using rat

although I am using cpp, I only use std::data struct, still go C style
std::queue don't provite interface to look at internel data, like begin method
so don't support for(auto i: std::queue)
std::deque

void myPerror(const char* errorMessage){
  fprintf(stderr, errorMessage); // potential insecure
}

#### a pipeline design
code change over development, it could hardly tell original design attention
```seudocode
  // BUSY_NEW is set by last stage when last stage successful copy data to this stage
  IF busy == BUSY_NEW
    busy = BUSY_DELAY

  IF busy > BUSY_DONE(1)
    // do the job
    busy--

  IF busy == BUSY_DONE
    // try to copy to next stage
    if success
      set nextStage->busy=BUSY_NEW
```

original look
```shell
  typein command: initialize(r), simulate(sim) <n>, display(p), quit(q)
  (apex) >>n
  --------------------------------
  after Clock Cycle #: 4
  --------------------------------
  ROB            : pc(4000) MOVC,R1[U00 -1],#1
  ROB            : pc(4004) MOVC,R2[U01 -1],#2
  intFU          : pc(4000) MOVC,R1[U00 -1],#1  UNSTALLED -1
  IQ         [00]: pc(4004) MOVC,R2[U01 -1],#2
  DRD            : pc(4008) MOVC,R3[U-1 -1],#3  UNSTALLED 10
  Fetch          : pc(4012) ADD,R1[U-1 -1],R2[U-1 -1],R2[U-1 -1]  UNSTALLED 10

  typein command: initialize(r), simulate(sim) <n>, display(p), quit(q)
  (apex) >>quit

```

what I want
```shell
  typein command: initialize(r), simulate(sim) <n>, display(p), quit(q)
  (apex) >>n
  ================================
  after Clock Cycle #: 4
  ================================
             ROB : pc(4000,124,2) MOVC,R1[U00 -1],#1
             ROB : pc(4004,124,2) MOVC,R2[U01 -1],#2

  MEM            : pc(4000,124,2) STORE,R1[U00 -1],#1  UNSTALLED -1
             LSQ : pc(4000,124,2) LOAD,R1[U00 -1],#1  UNSTALLED -1
             LSQ : pc(4000,124,2) LOAD,R1[U00 -1],#1  UNSTALLED -1
             LSQ : pc(4000,124,2) STORE,R1[U00 -1],#1  UNSTALLED -1
             LSQ : pc(4000,124,2) STORE,R1[U00 -1],#1  UNSTALLED -1

  intFU          : pc(4000,124,2) MOVC,R1[U00 -1],#1  UNSTALLED -1
  mulFU          : pc(4000,124,2) MOVC,R1[U00 -1],#1  UNSTALLED -1
           IQ[00]: pc(4004,124,2) MOVC,R2[U01 -1],#2
           IQ[02]: pc(4004,124,2) MOVC,R2[U01 -1],#2
           IQ[04]: pc(4004,124,2) MOVC,R2[U01 -1],#2
           IQ[03]: pc(4004,124,2) MOVC,R2[U01 -1],#2

  DRD            : pc(4008,124,2) MOVC,R3[U-1 -1],#3  UNSTALLED 10
  Fetch          : pc(4012,124,2) ADD,R1[U-1 -1],R2[U-1 -1],R2[U-1 -1]  UNSTALLED 10

  typein command: initialize(r), simulate(sim) <n>, display(p), quit(q)
  (apex) >>quit

```

print_regs
```shell
ARF[00] 000 001 002 003 004 005 006 007
ARF[08] 008 009 010 011 012 013 014 015
```
