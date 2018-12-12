/*
 * cpu.h
 * Contains various CPU and Pipeline Data structures
 * public to main.c, public interface of cpu module
 */

#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_

typedef struct APEX_CPU APEX_CPU;
typedef struct CPU_Stage CPU_Stage;

APEX_CPU*
APEX_cpu_init(const char* filename);

int
APEX_cpu_run(APEX_CPU* cpu);

void
APEX_cpu_stop(APEX_CPU* cpu);

#endif /* _APEX_CPU_H_ */
