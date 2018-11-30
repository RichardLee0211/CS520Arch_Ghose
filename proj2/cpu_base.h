#ifndef CPU_BASE_H
#define CPU_BASE_H

#include"global.h"

#include<inttypes.h>

/* cpu module-wise data */
enum
{
  F = 0,
  DRF,
  EX,
  MEM,
  WB,
  NUM_STAGES
};


/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
  char opcode[128];	// Operation Code
  int rd;		    // Destination Register Address
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int imm;		    // Literal Value
  // int delay[NUM_STAGES]; // wenchen: indicate delay in each stage, should be from the config file of pipeline and ISA
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
  // int delay[NUM_STAGES]; // wenchen: indicate delay in each stage
  int busy;		    // Flag to indicate, stage is performing some action
  int stalled;		// Flag to indicate, stage is stalled
} CPU_Stage;

/* Model of APEX CPU */
typedef struct APEX_CPU
{
  int clock;  /* Clock cycles elasped */
  int pc; /* Current program counter */
  uint32_t flags; /* 32 bits of flags, flags&0x1 is zero flag */
  int regs[NUM_REGS]; /* Integer register file */ // wenchen: make it 33 and regs[0] is not used
  int regs_valid[NUM_REGS];
  CPU_Stage stage[NUM_STAGES]; /* Array of 5 CPU_stage */
  int code_memory_size;
  APEX_Instruction* code_memory; /* Code Memory where instructions are stored */
  int data_memory[DATA_MEM_SIZE]; /* Data Memory */
  int ins_completed; /* Some stats */

} APEX_CPU;


/* basic functions */
int setStagetoNOPE(CPU_Stage* stage);
int copyStagetoNext(APEX_CPU* cpu, int stage_num);

/* print/debug functions */
void print_instruction(CPU_Stage* stage);
void print_stage_content(char* name, CPU_Stage* stage);
void print_all_stage(CPU_Stage* stages);
void print_regs(int* regs, int* regs_valid);
void print_data_memory(int* data_memory);
void print_flag_reg(uint32_t* flag_reg);

#endif /* CPU_BASE_H */
