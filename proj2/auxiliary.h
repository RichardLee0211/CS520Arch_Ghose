#ifndef _AUXILIARY_H_
#define _AUXILIARY_H_

#include"cpu.h"

int get_code_index(int pc);
int get_pc(int code_index);
static void print_instruction(CPU_Stage* stage);
static void print_stage_content(char* name, CPU_Stage* stage);
void print_all_stage(CPU_Stage* stages);
void print_regs(int* regs, int* regs_valid);
void print_data_memory(int* data_memory);
void print_flag_reg(uint32_t* flag_reg);

#endif /* _AUXILIARY_H_ */
