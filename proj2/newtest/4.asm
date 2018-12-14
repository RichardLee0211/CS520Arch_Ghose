# this is a crazy jump test
MOVC,R0,#15
MOVC,R1,#5
MOVC,R15,#0
MOVC,R2,#4000
# goto 4036: function
JAL,R3,R2,#36
# R15=1->2->3
ADDL,R15,R15,#1
# R0=15->10->5->0
SUB,R0,R0,R1
BNZ,#-16
HALT,
# function
MOVC,R7,#70
JUMP,R3,#0
# end of function
ADDL,R10,R15,#0
