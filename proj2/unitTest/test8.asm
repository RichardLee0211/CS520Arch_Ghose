# fabonacci array
# after excutioning this code, MEM[1..45] should be 1,1,2,3,5,8,13,21,34,55,89,144...
MOVC, R1, #1
MOVC, R2, #1
# data_mem_pointer
MOVC, R6, #1
# constant
MOVC, R5, #1
MOVC, R7, #0
# store mem[1]=1
# store mem[2]=1
STORE, R1, R6, #0
ADD, R6, R6, R5
STORE, R2, R6, #0
# counter of loop
MOVC, R4, #45
ADD, R3, R1, R2
ADD, R6, R6, R5
STORE, R3, R6, #0
ADD, R1, R2, R7
ADD, R2, R3, R7
SUB, R4, R4, R5
BZ #12
BNZ, #-28
MOVC R11 #11
MOVC R12 #12
HALT
