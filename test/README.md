this is for a design problem

unit1.1.h: #include"unit1.2.h"
unit1.1.c

unit1.2.h: #include"unit1.1.h"
unit1.2.c

then there is a undeterminated conditional directive

solution1: move to global.h and global.h
solution2: put unit1.1 and unit1.2 to one header file and multiple implement file
