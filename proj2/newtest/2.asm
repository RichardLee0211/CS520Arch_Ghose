# sorry to fail in clear mulFU 
MOVC,R0,#4000
MOVC,R1,#4
MOVC,R2,#1
# R3=5
ADD,R3,R1,R2
# R4=16
MUL,R4,R1,R1
# R5=15
SUB,R5,R4,R2
# R6=16
AND,R6,R2,R4
BZ,#-24
# R15=5
ADD,R15,R2,R1
# R5=15->10->5->0
SUB,R5,R5,R3
BNZ,#-8
# !! failed here, forgot to remove mulFU and intFU here
# R6=5*16=80
# MUL,R6,R3,R4
ADD,R6,R3,R4
JAL,R14,R0,#64
# R7=10
ADD,R7,R15,R15
# R8=20
ADD,R8,R7,R7
HALT,
# R9=30
MOVC,R9,#30
JUMP,R14,#0
# what's this?? can't take this instrn
# SUB,R13,#13
