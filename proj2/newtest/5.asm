MOVC,R0,#0
MOVC,R1,#1
MOVC,R2,#2
MOVC,R3,#1212
# R4=2
MUL,R4,R2,R1
MUL,R4,R2,R1  
# mem[44]=2
STORE,R4,R0,#44
# R6=R5=mem[44]=2
LOAD,R5,R4,#42
LOAD,R6,R4,#42
# mem[24]=2
STORE,R6,R6,#22
# R7=2424
MUL,R7,R2,R3
MOVC,R9,#606
# R10=2424+606=3030
ADD,R10,R7,R9
BNZ,#16
MOVC,R11,#100
MOVC,R12,#200
MOVC,R13,#300
HALT,
