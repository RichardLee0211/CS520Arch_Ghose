# write for testing data forwarding
# comment, if not support, remove them
MOVC r1 #1
MOVC r2 #2
# data forwarding from prior instrn, shouldn't get bubble here
MUL r3 r1 r2
SUB r4 r1 r3
ADD r5 r1 r4
# data forwarding from load rd, should get a bubble here, because LOAD get result for MEM stage
LOAD r6 r1 #0
ADD r7 r6 r1
# continus update r3, shouldn't get NOP here
ADD r3 r1 r2
MOVC r3 #33
ADD r3 r1 r2
ADD r3 r1 r2
LOAD r3 r3 #123
# special command, expect one bubble here
MUL r3 r1 r2
ADD r3 r3 r3
SUB r3 r3 r3
ADD r3 r3 r2
SUB r3 r3 r1
# condition JUMP
BNZ #-4
# JUMP R1 #-9
