#ifndef CPU_BASE_H
#define CPU_BASE_H

#include"global.h"

#include<inttypes.h>
#include<queue>
#include<deque>

/* cpu module-wise data */
enum
{
  F = 0,
  DRD,
  IQ,
  intFU,
  mulFU,
  LSQ,
  MEM,
  ROB,
  NUM_STAGES,
  // following is not used
  DRF,
  EX,
  WB
};


/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
  char opcode[OPCODE_SIZE];	// Operation Code
  int rd;		    // Destination Register Address
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int imm;		    // Literal Value
} APEX_Instruction;

/* old Model of CPU stage latch */
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
  int buffer;		// Latch to hold result of FU for later
  int mem_address;	// Computed Memory Address, for LOAD, STORE, JUMP, JAL, BZ, BNZ
  int busy;		    // Flag to indicate, stage is performing some action
  int stalled;		// Flag to indicate, stage is stalled
} CPU_Stage;

struct CPU_Stage_base
{
  int stage_valid;

  int pc;		    // Program Counter
  char opcode[OPCODE_SIZE];	// Operation Code
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int rd;		    // Destination Register Address
  int imm;		    // Literal Value

  int rs1_tag;
  int rs2_tag;
  int rd_tag;

  int rs1_value;	// Source-1 Register Value
  int rs2_value;	// Source-2 Register Value
  int rs1_value_valid;
  int rs2_value_valid;

  int buffer;		// Latch to hold result of FU for later
  int buffer_valid;
  int mem_address;	// Computed Memory Address, for LOAD, STORE, JUMP, JAL, BZ, BNZ
  int mem_address_valid;

  int dispatch_cycle; // this is like unique ID of instrn after dispatch, could use it to find conresponding instrn in LSQ, ROB and IQ
  int CFID;         // for branch instrns
  // int ROB_index; // using dispatch_cycle to do ID
  // int IQ_index;
  // int LSQ_index;
};

struct Fetch_t{
  // CPU_Stage_base latch;
  CPU_Stage_base stage;
  int busy;
  int stalled;
};

struct DRD_t{
  CPU_Stage_base latch;
  CPU_Stage_base stage;
  int busy;
  int stalled;
};

struct IQ_t{
  CPU_Stage_base entry[NUM_IQ_ENTRY];
  int busy;
  int stalled;
};

struct IntFU_t{
  CPU_Stage_base latch;
  CPU_Stage_base stage;
  int busy;
  int stalled;
};

struct MulFU_t{
  CPU_Stage_base latch;
  CPU_Stage_base stage;
  int busy;
  int stalled;
};

struct LSQ_t{
  std::deque<CPU_Stage_base> entry; // size should not greater then NUM_LSQ_ENTRY
  int busy;
  int stalled;
};

struct ROB_t{
  std::deque<CPU_Stage_base> entry; // size should not greater than NUM_ROB_ENTRY;
  int busy;
  int stalled;
};


/* Model of APEX CPU */
struct APEX_CPU
{
  int clock;  /* Clock cycles elasped */
  int pc; /* Current program counter */

  // TODO: maybe remove it
  uint32_t flags; /* 32 bits of flags, flags&0x1 is zero flag */

  /* TODO: could be removed */
  int regs[NUM_REGS]; /* Integer register file */ // wenchen: make it 33 and regs[0] is not used
  int regs_valid[NUM_REGS];

  int rat[NUM_REGS]; /* register alias table */
  int r_rat[NUM_REGS]; /* retired rat */

  /* unify regster file */
  int urf[NUM_UREGS];
  int urf_valid[NUM_UREGS]; // function like free list
  int urf_z_flag[NUM_UREGS];
  // int urf_free[NUM_UREGS];

  /* fetch, DRD, IQ, ROB, intFU, mulFU */
  CPU_Stage stage[NUM_STAGES]; /* Array of 5 CPU_stage */
  Fetch_t fetch_stage;
  DRD_t drd;
  ROB_t rob;
  LSQ_t lsq;
  IQ_t iq;
  IntFU_t intFU;


  int code_memory_size;
  APEX_Instruction* code_memory; /* Code Memory where instructions are stored */
  int data_memory[DATA_MEM_SIZE]; /* Data Memory */
  int ins_completed; /* Some stats */

};


/* basic functions */
int setStagetoNOPE(CPU_Stage* stage); // old
int setStagetoNOP(CPU_Stage_base* stage);
int copyStagetoNext(APEX_CPU* cpu, int stage_num);

/* print/debug functions */
void print_instruction(CPU_Stage* stage);
void print_stage_content(char* name, CPU_Stage* stage);
void print_all_stage(CPU_Stage* stages);
void print_all_stage(APEX_CPU* cpu);
void print_regs(int* regs, int* regs_valid);
void print_data_memory(int* data_memory);
void print_flag_reg(uint32_t* flag_reg);

int Fetch_stage_init(Fetch_t* stage);
int DRD_init(DRD_t* stage);
int IQ_init(IQ_t *stage);
int ROB_init(ROB_t* stage);
int IntFU_init(IntFU_t* stage);

#endif /* CPU_BASE_H */
