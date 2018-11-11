MOVC, R1, #1
MOVC, R2, #1

MOVC, R6, #1 // data_mem_pointer
// constant
MOVC, R5, #1
MOVC, R7, #0

STORE, R1, R6, #0
ADD, R6, R6, R5
STORE, R2, R6, #0

MOVC, R4, #10 // counter
ADD, R3, R1, R2 // update R3
ADD, R6, R6, R5 // update data_mem_pointer
STORE, R3, R6, #0 // store R3
ADD, R1, R2, R7 // update R1
ADD, R2, R3, R7 // update R2
SUB, R4, R4, R5 // update counter
BNZ, #-24
HALT
