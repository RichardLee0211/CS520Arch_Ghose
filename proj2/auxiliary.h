/* it's cpu_helper file, some auxiliary funcitons */
#ifndef _AUXILIARY_H_
#define _AUXILIARY_H_

#include<limits.h>
#include<inttypes.h>

/* helper functions */
int get_code_index(int pc);
int get_pc(int code_index);
int copyArray(int* arr1, int* arr2, int size);

#endif /* _AUXILIARY_H_ */
