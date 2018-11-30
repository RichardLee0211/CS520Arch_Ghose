this is for a design problem

unit1.1.h: #include"unit1.2.h"
unit1.1.c

unit1.2.h: #include"unit1.1.h"
unit1.2.c

then there is a undeterminated conditional directive

solution1: move to global.h and global.h
solution2: put unit1.1 and unit1.2 to one header file and multiple implement file

### something about file organization
I don't know if the number is right, but we have to separate unit from unit, layer from layer.
otherwise, we will have one file with billion lines of code to do the jobs. That's not a right way for a human to organize code

separate unit from unit, by functionality
separate layer from layer, by operation level
    e.g. one mode, in C style
    unit1.h: define high level data structure and high level functions
    unit1_helper.h: define low level helper funcitons and print/debug funcitons,
                    when print/debug functions needs unit1 data structure pointer, using forwarding decl
    unit1.c: #include: unit1.h unit1_helper.h
    unit1_helper.c: #include"unit1_helper.h"

separate function from function, a function would about 120~500 lines
a file would be about 1200~3600 lines

yes, gcc -c will compile module without knowing data structure and function implement. complie stage only need to know declare
guess linker will link all the declare with implement code(defination)
one defination rule, multiple declarations

### reading "CHeaderFileGuidelines.pdf" from [here](http://umich.edu/~eecs381/handouts/CHeaderFileGuidelines.pdf)
