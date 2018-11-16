the name of original fold:
simple_apex_pipeline_template_code_store_movc_instructions

strtok(char*, ",")
atoi(char*)
atol(char*)
atof(char*)

not in the C std document
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

- [ ] MOVC RD #NUM
// - [ ] MOV RD R1

- [ ] ADD RD R1 R2
- [ ] SUB RD R1 R2
- [ ] MUL RD R1 R2
// - [ ] DIV RD R1 R2

- [ ] AND RD R1 R2
- [ ] OR RD R1 R2
// - [ ] NOT RD R1
- [ ] EX-OR RD R1 R2

- [ ] LOAD RD R2 #NUM
- [ ] STORE R1 R2 #NUM

- [ ] BZ, #-12
- [ ] BNZ, #12
- [ ] JUMP, RS1, #12
- [ ] HALT

// may not be a good idea when it comes to out of order excution
- [ ] PRINT_REG R1
- [ ] PRINT_MEM R1 #NUM
- [ ] NOP

for each stage:
resource: 1 0
busy: 1(>0), 0( <=0)
next_stage.stalled || next_busy>0: 1 0

011: stalled_1, stalled_2
010: stalled_1, copy NOP to next_stage
111: process
110: process, copy NOP to next_stage
101: stalled_2
100: copy stage to next_stage

```sudoCode
  IF resource UNVALID(not belong to this instruction at this stage)
    stage->stall = true
  ELSE IF stage->busy > 0
    allocate resource(mark it UNVALID and belong to this instruction)
    process;
    stage->busy--;
    stage->stall=false; // not stall because of resource

  IF next_stage->busy <= 0 && !next_stage->stalled
    IF stage->busy <= 0
      free resource(mark it VALID)
      next_stage = stage
    ELSE
      next_stage = NOP
  ELSE
    stage->stalled=true
```

```sudoCode
  FETCH()
    IF stage->stalled
      // don't fetch from instr_mem
    ELSE IF pc is out of boundary
      fetch NOP
    ELSE
      fetch instruction in ins_mem
      stage->busy--;
      stage->stall=false; // not stall because of resource
    /* copy to next_stage */
    IF next_stage->busy <= 0 && !next_stage->stalled
      IF stage->busy <= 0
        next_stage = stage
      ELSE
        next_stage = NOP
    ELSE
      stage->stalled=true

  DECODE()
    IF all register is valid  or belong to this instruction
      mark RD_valid = stage->pc
      fetch value from reg_file to stage
      stage->busy--
      stage->stalled = false
    ELSE
      stage->stall = true
    /* copy to next_stage */
    IF next_stage->busy <= 0 && !next_stage->stalled
      IF stage->busy <= 0
        next_stage = stage
      ELSE
        next_stage = NOP
    ELSE
      stage->stalled=true

  EX()
    // later, could have FU to allocate
    IF stage->busy>0
      process
      stage->busy--
      stage->stall =false
    /* copy to next_stage */
    IF next_stage->busy <= 0 && !next_stage->stalled
      IF stage->busy <= 0
        next_stage = stage
      ELSE
        next_stage = NOP
    ELSE
      stage->stalled=true

  MEM()
    IF stage->busy>0
      load or store from data_mem
      stage->busy--
      stage->stall =false
    /* copy to next_stage */
    IF next_stage->busy <= 0 && !next_stage->stalled
      IF stage->busy <= 0
        next_stage = stage
      ELSE
        next_stage = NOP
    ELSE
      stage->stalled=true

  /* WB stage seems don't have reason for a long delay */
  WB()
    IF stage->busy>0
      update reg_file
      stage->busy--
      stage->stall =false
    IF stage->busy<=0
      RD_valid = true




```

the original code don't make sense
```sudoCode
  IF !stage->busy && !stage->stall
    ... // process
    ELSE IF stage->opcode="MUL"
      stage->busy=1
    ELSE ...

  IF next_busy<=0 || !next_stage->stalled
    IF stage->busy<=0
      copy stage to next_stage
    ELSE
      next_stage = NOP
  ELSE
    stage->stalled=true
```

DONE: interlock
DONE: this is not right, when pc is excess the boundary of code_memory_size, it keep fetch the garbadge
TODO: busy and stalled is in the stage struct which is also latch
TODO: flag register
DONE: command interface
- initialize: init
- Simulate <n>: step
- display
  DONE: print flag register
DONE: part B

## others
### these don't matter
TODO: I don't like the comma
TODO: don't like the print message
TODO: clock and pc is int

share variables between C file
A.h: extern int i;
A.c: int i; // global
A.c: #include<A.h>

if data is from clients, code got to be handle whole possible space
if data is design data, get to leave some modifiable space
if data is provided by lower level, ask them and make sure
don't hard code constant in code file, make them all together in a single file, called config file
code is only about logic

DONE: VALID=0, regs_valid=pc
TODO: return to invoke the old command

when copy NOP to stage(bubble), set pc=0
when pc>=get_pc(cpu->code_memory_size), set F stage to NOP, and pc=get_pc(cpu->code_memory_size)
end pipline when WB get (stage->pc >= cpu->code_memory_size)
ins_completed++ seems to useless at this point



## why do we need to set zero flag in WB stage?
It confused me. setting the zero flag in WB stage will introduce many complex and gain no benefit by doing so. For example:
1. If setting the zero flag in WB stage, the code needs to judge whether to generate a bubble or not.
  e.g. ADD; BZ;
  it needs to generate a bubble and become ADD; NOP; BZ; to make sure ADD is at WB stage and set the ﻿zero flag, and BZ is in EX stage read the ﻿zero flag.
  e.g. ADD; AND; BZ;
  it doesn't need to  generate a bubble to make sure that.
2. proj1 is an in order pipeline, setting the zero flags in WB stage or EX stage doesn't change zero flag's state in a program
3. if I use zero_flag and zero_flag_valid, making zero_flag a resource for ADD, SUB, MUL, to judge whether BZ and BNZ wait in EX stage, then it slows down a sequence of ADD, SUB, and MUL.
  e.g. ADD; BZ;
  ADD in EX stage, set zero_flag_valid=INVALID
  ADD in MEM, BZ in EX find zero_flag_valid=INVALID, wait and generate a bubble for next clock
  ADD in WB, set sero_flag, set zero_flag_valid=VALID; NOP in MEM stage; BZ in EX stage read zero_flag_valid=VALID, read zero_flag.
  e.g. the same logic for ADD R1 R2 R3; SUB R4 R5 R6;
  ADD in EX, set zero_flag_valid = INVALID
  ADD in MEM; SUB in EX read zero_flag_valid=INVALID, wait
  ADD in WB, set zero_flag_valid = VALID and set zero flag; NOP in MEM; SUB in EX progress
It introduces an unnecessary bubble.
In conclusion, in this in-order pipeline, why not set the zero flag in EX stage, then BZ following to read zero flag?

## assert.h
The canonical way:
1) Use #include <assert.h> and call assert() in your code.
2) Then at build time:
2a) gcc blablabla : this is a debug build, NDEBUG is not defined, and assert() goes into action.
2b) gcc blablabla -DNDEBUG blablabla : this is a production build, NDEBUG is defined, assert() do nothing.

## original code sucks
I don't know if it's what you asked for, but you could do the following to improve the skeleton code and design:
- add comment parse in file_parser.c, it's hard to write long asm code without comment
- make parser compatible with ADD R1 R2 R3, I don't know who writes asm code in form of ADD,R1,R2,R3. It just gets under my skin when I look at it.
- stop the F stage keeping fetch garbadge from code_memory after it execess boundary, it's a huge vuln
- stop condition is not good after BZ, BNZ, JUMP are introduced
- make the interactive environment a choice instead of the default setting by adding -i parameter .
-

I doubt the skeleton code and test case are not done by the same person, several stupid errors are made.
first, need get the file_parser right

this is not beautiful,
make stupid modification for not data forwarding and BZ, to check stage[EX] stage[MEM] stage[WB] have ADD, SUB, MUL or not
find a data forwarding bug,
if depents on LOAD, then have to look at stage[MEM+1]
if depents on ADD, SUB, MUL, AND, OR, EX-OR, NOT, looking at stage[EX+1], stage[MEM+1], or this is valid

and goto stalled code structure
has too many little cases inside big cases that could lead to the same result

this is why I don't like have multi_backs up, when you find a buf, got to update them all by hand

dependence on LOAD rd rs1 rs2

Data forwarding logic:
in DRF stage:
  for RS1
  IF VALID
    copy from reg_file
  ELSE IF EX && EX stalled
    STALLED
  ELSE IF EX+1
    IF EX+1 == LOAD
      STALLED
    ELSE
      copy from EX+1
  ELSE IF MEM+1
    copy from MEM+1
  ELSE
    ASSERT

register renaming, don't need to check RD valid or not, just update reg_valid

### notes about my test cases
add space and \n as token_clim
add command detector
add empty line detector
