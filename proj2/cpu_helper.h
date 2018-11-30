/* it's cpu_helper file, some auxiliary funcitons */
#ifndef CPU_HELPER_H
#define CPU_HELPER_H

/* helper functions */
int get_code_index(int pc);
int get_pc(int code_index);
int copyArray(int* arr1, int* arr2, int size);

#endif /* CPU_HELPER_H */
