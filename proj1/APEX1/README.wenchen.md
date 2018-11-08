the name of original fold:
simple_apex_pipeline_template_code_store_movc_instructions

strtok(char*, ",")
atoi(char*)
atol(char*)
atof(char*)

not in the C std document
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

- [ ] MOVC RD #NUM
// - [ ] MOV RD #NUM

- [ ] ADD RD R1 R2
- [ ] SUB RD R1 R2
- [ ] MUL RD R1 R2
// - [ ] DIV RD R1 R2

- [ ] AND RD R1 R2
- [ ] OR RD R1 R2
// - [ ] NOP?? RD R1
- [ ] EX-OR RD R1 R2

- [ ] LOAD RD R2 #NUM
- [ ] STORE R1 R2 #NUM

- [ ] BZ
- [ ] BNZ
- [ ] JUMP
- [ ] HALT

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
