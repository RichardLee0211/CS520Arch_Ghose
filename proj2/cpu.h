#include "file_parser.h"

/* wenchen */
#include<limits.h>
#include<inttypes.h>


#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_
/**
 *  cpu.h
 *  Contains various CPU and Pipeline Data structures
 */


typedef unsigned int uint;

/* wenchen regster valid bits */
#define NUM_REGS 32 /* R0~R31, require R0~R15 */
#define VALID 0

/* unused value in instruction struct */
#define UNUSED_REG_INDEX -1
#define UNUSED_IMM INT_MAX

/* convert pc to code index */
#define PC_START_INDEX 4000
#define BYTES_PER_INS 4

/* the state of stage in cpu */
#define STALLED 1
#define UNSTALLED 0
#define BUSY 1
#define UNBUSY 0

#define DATA_MEM_SIZE 4096

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1
// #define FORWARD_ENABLE 1

enum
{
  F = 0,
  DRF,
  EX,
  MEM,
  WB,
  NUM_STAGES
};

extern int enable_interactive;
extern int default_delay[NUM_STAGES];


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
  // uint32_t flags_valid; /* 32 bits of flags_valid, flags&0x1 is zero flag */
  int regs[NUM_REGS]; /* Integer register file */ // wenchen: make it 33 and regs[0] is not used
  int regs_valid[NUM_REGS];
  CPU_Stage stage[NUM_STAGES]; /* Array of 5 CPU_stage */
  int code_memory_size;
  APEX_Instruction* code_memory; /* Code Memory where instructions are stored */
  int data_memory[DATA_MEM_SIZE]; /* Data Memory */
  int ins_completed; /* Some stats */

} APEX_CPU;

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

/* wenchen */
int
copyArray(int* arr1, int* arr2, int size);

#endif
