MOVC, R1, #11
MOVC, R2, #22
ADD, R3, R1, R2
SUB, R4, R2, R1
MUL, R5, R3, R4
AND, R6, R5, R4
OR, R7, R5, R4
# expect
# R3 = 33
# R4 = 11
# R5 = 363
# R6 = 363
# R7 = 11
