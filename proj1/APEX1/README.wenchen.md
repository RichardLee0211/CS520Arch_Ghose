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

TODO: interlock
TODO: this is not right, when pc is excess the boundary of code_memory_size, it keep fetch the garbadge
TODO: busy and stalled is in the stage struct which is also latch
TODO: flag register
TODO: command interface
- initialize: init
- Simulate <n>: step
- display
  TODO: print flag register
TODO: part B

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
