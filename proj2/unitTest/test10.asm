# try to exhaust all cfid
MOVC R0 #0
MOVC R1 #1
MOVC R10 #20
SUB R10 R10 R1
BZ #8
BNZ #-8
MOVC R11 #11
HALT
