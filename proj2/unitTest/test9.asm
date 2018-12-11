# this is crazy branch test case
# try to trigger U_z_flag reuse problem if implement with urf_z_flag[3]
MOVC R0 #0
MOVC R1 #1
MOVC R2 #2
SUB R3 R2 R1
EX-OR R3 R3 R3
EX-OR R3 R3 R3
EX-OR R3 R3 R3
EX-OR R3 R3 R3
EX-OR R3 R3 R3
BZ #12
BNZ #12
# this JUMP will never excuted
JUMP R0 #-12
# if BZ, R10=10; if BNZ, R11=11
MOVC R10 #10
MOVC R11 #11
HALT
