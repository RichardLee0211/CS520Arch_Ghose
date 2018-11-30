it's gonna be a big project, go get it

### data structure
- URF: R0~R39, unified register file
    extension to hold the Z flag
    one-bit status flag that indicates if their contents are valid
    the free bit the indicates if the entry is free
- ARF: R0~R15
    PRF: ??
- RAT: register alias table
- back-RAT: back-end register alias table??

- IQ: 16 entries
    field to hold the cycle count at the time of dispatch
- ROB: 32 entries
- LSQ: 20 entries

tag: URF index
stage: src1_valid src1_tag

broadcast_tag
broadcast_data

IF R_RAT[src1] != INVALID
    pickup from ARF[R_RAT[src1]]
    stage->src1_valid = VALID
ELSE IF RAT[src1] != INVALID
    pickup from URF[RAT[src1]]
    stage->src1_valid = VALID
ELSE IF

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
- [ ] HALT
- [ ] JAL RD R1 #NUM
        RET implement as JUMP RD #0 /* RD saves the next addr of JAL */

// may not be a good idea when it comes to out of order excution
// - [ ] PRINT_REG R1
// - [ ] PRINT_MEM R1 #NUM
- [ ] NOP

### questions
- where renaming actually happen, D/RF or IQ ?:
  DRD stage, decode/renaming/dispatch
- what data Struct RAT and R_RAT points to ?:
  RAT[R1] = UX, R_RAT[R1] = UY, after RAT[R1] is update, save old value in R_RAT[R1], used for recovering
- how file_parse.c functions become useable in cpu.c, without include ?:
    in the original code, public file_parser function is declared in cpu.h, it's like one .h file and two implement .c file
- don't need write back stage?? do wirte back operations in the end of intFU, mulFU, memFU

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
