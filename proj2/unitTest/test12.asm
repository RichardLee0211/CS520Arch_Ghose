# STORE R1 R2 #3
# STORE wait in IQ for R2 only
# could get R1 when STORE is in intFU, in LSQ,
# another: STORE must wait in LSQ until its ROB entry is in the head of ROB
MOVC R0 #0
MOVC R1 #1
MOVC R2 #2
MUL R3 R2 R2
MUL R3 R2 R2
MUL R3 R2 R2
MUL R3 R2 R2
MUL R3 R2 R2
STORE R3 R0 #0
