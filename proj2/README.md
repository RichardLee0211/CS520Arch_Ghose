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

### about branch_instrn
#### way 1:
using
int urf_z_flag[NUM_UREGS]
int urf_z_flag_valid[NUM_UREGS]

when an arithmetic instrn dispatch,
    cfid_arr[cfid].z_urf_index = rd_tag;
    urf_z_flag_valid[rd_tag]=INVALID; // let BZ/BNZ wait in IQ

when this arithmetic instrn finish EX,
    tell cfid_arr[cfid] z_flag is ready, and z_flag value; // let BZ/BNZ waiting in IQ readyforIssue

when this arithmetic instrn commited in ROB,
    urf_z_flag_valid[rd_tag] = VALID; //
    urf_z_flag[rd_tag] = z_flag;

to handle test case like this:
[0] LOAD R2 R1 #0
[0] MUL R3 R2 R2   # let RAT[R3]=U3, when this instrn commited, R_RAT[R3]=U3, set urf_z_flag[U3]=value
[0] EX-OR R3 R2 R2 # let RAT[R3]=U4, when this instrn commited, R_RAT[R3]=U4, U3 is free
[0] EX-OR R3 R2 R2
[0] EX-OR R3 R2 R2
[0] EX-OR R3 R2 R2 # because urf allocation logic start from urf[0] to urf[39] check for free entry, it's highly possible to allocate U3 again
[0] EX-OR R3 R2 R2 # let RAT[R3]=U3, when this instrn commited, R_RAT[R3]=U3, not set urf_z_flag[U3] because this is not a arithmetic instrn
[0] EX-OR R3 R2 R2
[0] BZ #4          # when it's in intFU, look at cfid_arr[0].z_urf_index (U3), and urf_z_flag[U3] to judgement
[1] BNZ #4

way too complicated

#### way 2:
  int arithmetic_count;
  // the number of arithmtic instrn in the flight,
  // initial to 0, when it's 0, BZ or BNZ could be issue to intFU
  kind of stupid


#### way z:
using CFID_arr to store last arithmetic instrn status and z_flag value
  int z_flag; // z_flag from last arithmetic instrn, initial to VALID
  int z_flag_valid; // initial to VALID, when an arithmetic instrn dispatch, set to INVALID; when this instrn finished EX, set to VALID
  int z_urf_index; // record the rd_tag of last arithmetic instrn, used for set z_flag_valid to VALID
1.
JUMP R1 #23
JAL Rd R1 #23
JUMP/JAL is readyforIssue when it gets all resource register value
when JUMP/JAL is in intFU, need to Flush instrn according to cfid and cfio
2.
BZ/BNZ is readyforIssue when associated cfid_arr entry get valid z_flag
when an arithmetic instrn dispatch:
     cfid_arr[cfid]->z_flag_valid = INVALID // telling coming BZ/BNZ to wait in IQ
     cfid_arr[cfid]->z_urf_index = rd_tag

when this arithmetic finish calulation:
     for all rd_tag == cfid_arr[i].z_urf_index,
       set cfid_arr[i].z_flag and cfid_arr[i].z_flag_valid=VALID

when a branch_instrn dispatch and allocate a new CFID:
    cfid_arr[new_cfid].z_flag_valid = cfid_arr[old_cfid].z_flag_valid
    cfid_arr[new_cfid].z_flag = cfid_arr[old_cfid].z_flag
    cfid_arr[new_cfid].z_urf_index = cfid_arr[old_cfid].z_urf_index

when a branch_instrn check if it's readyforIssue:
  if BZ || BNZ
    if cfid_arr[cfid].z_flag_valid == VALID
      it's ready

To handle test case like this:
[0] LOAD R2 R1 #0
[0] MUL R3 R2 R2
[0] BZ #4
[1] BNZ #4

Or more crazy:
[0] LOAD R2 R1 #0
[0] MUL R3 R2 R2
[0] JUMP R0 #0
[1] BZ #4
[2] BNZ #4



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

#### A problem is,
when a instrn is completed in ROB but can't commited because of instrn before hand
then its rd_value get to have a way to forward to DRD and IQ stage
```shell
  ================================================================================
  after Clock Cycle #: 9
  ================================================================================
  ROB : pc(4008,005,0) STORE,R1[U01 -1],R0[U00 00],#0
  ROB : pc(4012,006,0) ADD,R2[U02 02],R1[U01 01],R1[U01 01]  COMPLETE
  ROB : pc(4016,007,0) ADD,R3[U03 04],R2[U02 02],R2[U02 02]  COMPLETE
  ROB : pc(4020,008,0) ADD,R4[U04 -1],R2[U02 02],R2[U02 02]
  ROB : pc(4024,009,0) ADD,R5[U05 -1],R2[U02 -1],R2[U02 -1]

  MEM            : pc(4008,005,0) STORE,R1[U01 01],R0[U00 00],#0   1
  LSQ : empty

  intFU          : pc(4020,008,0) ADD,R4[U04 -1],R2[U02 02],R2[U02 02]   10
  mulFU          : pc(0000,-01,-1) UNKNOWN   -1
  IQ[00] : pc(4024,009,0) ADD,R5[U05 -1],R2[U02 -1],R2[U02 -1]

  DRD            : pc(4028,-01,-1) ADD,R6[U-1 -1],R2[U-1 -1],R2[U-1 -1]   10
  Fetch          : pc(4032,-01,-1) ADD,R7[U-1 -1],R2[U-1 -1],R2[U-1 -1]   10

  typein command: initialize(r), simulate(sim) <n>, display(p, pu), quit(q)
  (apex) >>n
  ================================================================================
  after Clock Cycle #: 10
  ================================================================================
  ROB : pc(4008,005,0) STORE,R1[U01 01],R0[U00 00],#0  COMPLETE
  ROB : pc(4012,006,0) ADD,R2[U02 02],R1[U01 01],R1[U01 01]  COMPLETE
  ROB : pc(4016,007,0) ADD,R3[U03 04],R2[U02 02],R2[U02 02]  COMPLETE
  ROB : pc(4020,008,0) ADD,R4[U04 04],R2[U02 02],R2[U02 02]  COMPLETE
  ROB : pc(4024,009,0) ADD,R5[U05 -1],R2[U02 -1],R2[U02 -1]
  ROB : pc(4028,010,0) ADD,R6[U06 -1],R2[U02 -1],R2[U02 -1]

  MEM            : empty
  LSQ : empty

  intFU          : empty
  mulFU          : pc(0000,-01,-1) UNKNOWN   -1
  IQ[00] : pc(4024,009,0) ADD,R5[U05 -1],R2[U02 -1],R2[U02 -1]
  IQ[01] : pc(4028,010,0) ADD,R6[U06 -1],R2[U02 -1],R2[U02 -1]

  DRD            : pc(4032,-01,-1) ADD,R7[U-1 -1],R2[U-1 -1],R2[U-1 -1]   10
  Fetch          : pc(4036,-01,-1) ADD,R8[U-1 -1],R2[U-1 -1],R2[U-1 -1]   10

  typein command: initialize(r), simulate(sim) <n>, display(p, pu), quit(q)
  (apex) >>
```
broadcast need hold all finished instrn rd_value until this instrn is commited

#### here's why don't include function definition in header file
say you have cpu_base public for all sub-modules in cpu module
when you have a function definition in cpu_base.h
and because you include cpu_base.h in many cpu sub-modules
when cpu sub-module links, links would complain that you have duplicate symbols,
meaning you have multiply the same function definitions
don't definition functions in header files, even for operator< function

#### don't use malloc and C++ class together
this is kind of annoying when convert C code to C++
C Styple
```C
  My* pMy = (My*)malloc(sizeof(My));
  if(pMy != NULL){
    My_init(pMy);
  }
```

C++ style
```c++
  My* pMy = new My(); // initial is done
```

and when you have std::set in C struct, and use using malloc to allocate space,
then std::set is not initial correctly

this is a real mix of C and C++ style,
get the reference of a object and then get the addr of it as a pointer
```cpp
  for(auto& i: cpu->lsq.entry){
    if(strcmp(i.opcode, "STORE")==0)
      LSQ_STORE_fetchValue(cpu, &i);
  }
```

now I think useing memset to initial data is a bad idea, shouldn't save energy there

learn somthing about deque
// iterator could ++, --
auto it = deque.begin() // iterator
auto it = deque.end() // iterator
// reference
auto& ref = deque.front()
auto& ref = deque.back()
