# comment

MOVC r1 #1
MOVC r2 #2
# data forwarding from add rd
ADD r3 r1 r2
ADD r4 r1 r3
ADD r5 r1 r4
# data forwarding from load rd
LOAD r6 r1 #0
ADD r7 r6 r1
# output dependence
ADD r3 r1 r2
ADD r3 r1 r2
ADD r3 r1 r2
ADD r3 r1 r2
