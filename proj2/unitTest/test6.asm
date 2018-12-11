# testing BZ and BNZ
MOVC R0 #0
MOVC R1 #1
MOVC R2 #2
MOVC R3 #50
MOVC R5 #0
# loop like 50 times and increse R5 by 1
SUB R4 R2 R2
ADD R5 R5 R1
SUB R3 R3 R1
BZ #12
BNZ #-16
# R1 shouldn't change
ADD R1 R1 R1
MOVC R8 #8
HALT
