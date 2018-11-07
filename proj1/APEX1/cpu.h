#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_
/**
 *  cpu.h
 *  Contains various CPU and Pipeline Data structures
 *
 *  Author :
 *  Gaurav Kothari (gkothar1@binghamton.edu)
 *  State University of New York, Binghamton
 */

/* wenchen */
#include<limits.h>

/* wenchen regster valid bits */
#define NUM_REGS 33 // 32+1, since regs[0] is not used
#define VALID 1
#define UNVALID 0

/* unused value in instruction struct */
#define UNUSED_REG_INDEX 0
#define UNUSED_IMM INT_MAX

/* convert pc to code index */
#define PC_START_INDEX 4000
#define BYTES_PER_INS 4

/* the state of stage in cpu */
#define STALLED 1
#define UNSTALLED 0
#define BUSY 1
#define UNBUSY 0

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1


enum
{
  F = 0,
  DRF,
  EX,
  MEM,
  WB,
  NUM_STAGES // what's this?: number of stage, smart way
};

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
  char opcode[128];	// Operation Code
  int rd;		    // Destination Register Address
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int imm;		    // Literal Value
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
  int pc;		    // Program Counter
  char opcode[128];	// Operation Code
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int rd;		    // Destination Register Address
  int imm;		    // Literal Value
  int rs1_value;	// Source-1 Register Value
  int rs2_value;	// Source-2 Register Value
  int buffer;		// Latch to hold some value, like result of EX
  int mem_address;	// Computed Memory Address
  int busy;		    // Flag to indicate, stage is performing some action
  int stalled;		// Flag to indicate, stage is stalled
} CPU_Stage;

/* Model of APEX CPU */
typedef struct APEX_CPU
{
  int clock;  /* Clock cycles elasped */
  int pc; /* Current program counter */
  int regs[NUM_REGS]; /* Integer register file */ // wenchen: make it 33 and regs[0] is not used
  int regs_valid[NUM_REGS];
  CPU_Stage stage[NUM_STAGES]; /* Array of 5 CPU_stage */
  int code_memory_size;
  APEX_Instruction* code_memory; /* Code Memory where instructions are stored */
  int data_memory[4096]; /* Data Memory */
  int ins_completed; /* Some stats */

} APEX_CPU;

APEX_Instruction*
create_code_memory(const char* filename, int* size);

APEX_CPU*
APEX_cpu_init(const char* filename);

int
APEX_cpu_run(APEX_CPU* cpu);

void
APEX_cpu_stop(APEX_CPU* cpu);

int
fetch(APEX_CPU* cpu);

int
decode(APEX_CPU* cpu);

int
execute(APEX_CPU* cpu);

int
memory(APEX_CPU* cpu);

int
writeback(APEX_CPU* cpu);

#endif
