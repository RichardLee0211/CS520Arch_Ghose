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
  int valid; // work for IQ entry. LSQ and ROB using std::deque
  int readyforIssue; // work for IQ entry, set to VALID when ready for issue
  int completed;  // work for ROB entry when check for retirement,
                  // set to VALID by intFU, mulFU, and mem

  int pc;		    // Program Counter
  char opcode[OPCODE_SIZE];	// Operation Code
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int rd;		    // Destination Register Address
  int imm;		    // Literal Value

  int rs1_tag;  // set at DRD stage, by looking at RAT
  int rs2_tag;  // set at DRD stage, by looking at RAT
  int rd_tag;   // set at DRD stage, new UR by URF_getValidIndex

  int rs1_value;	// Source-1 Register Value, set at DRD or IQ
  int rs2_value;	// Source-2 Register Value, set at DRD or IQ
  int rs1_value_valid; // work for IQ stage
  int rs2_value_valid; // work for IQ stage

  int buffer;		// Latch to hold result of FU for later, Rd result
  int buffer_valid; // set by intFU, mulFU, MEM, but no one check it
  int mem_address;	// Computed Memory Address, for LOAD, STORE, JUMP, JAL, BZ, BNZ
  int mem_address_valid; // set by intFU, but not one check it

  int dispatch_cycle; // this is like unique ID of instrn after dispatch,
                      // could use it to find conresponding instrn in LSQ, ROB and IQ
                      // set at DRD stage
  int cfid;         // for branch instrns
  // int ROB_index; // using dispatch_cycle to do ID
  // int IQ_index;
  // int LSQ_index;
};

struct Fetch_t{
  CPU_Stage_base entry;
  int busy;
  int stalled;
};

struct DRD_t{
  CPU_Stage_base entry;
  int busy;
  int stalled;
};

struct IQ_t{
  CPU_Stage_base entry[NUM_IQ_ENTRY];
  int busy;
  int stalled;
};

struct IntFU_t{
  CPU_Stage_base entry;
  int busy;
  int stalled;
};

struct MulFU_t{
  CPU_Stage_base entry;
  int busy;
  int stalled;
};

struct LSQ_t{
  std::deque<CPU_Stage_base> entry; // size should not greater then NUM_LSQ_ENTRY
  int busy;
  int stalled;
};

struct MEM_t{
  CPU_Stage_base entry;
  int busy;
  int stalled;
};

struct ROB_t{
  std::deque<CPU_Stage_base> entry; // size should not greater than NUM_ROB_ENTRY;
  int busy;
  int stalled;
};

struct Broadcast_t{
  int data_intFU;
  int tag_intFU;

  int data_mulFU;
  int tag_mulFU;

  int data_mem;
  int tag_mem;
};

/* wired naming
 * entry->cfid is true cfid that come along with instrn, it is also index of cfid_arr
 * this is entry of cpu->cfid_arr, store rat_bak and urf_bak,
 * in case branch is taken, and need to retore this data to cpu->rat and cpu->urf
 *
 * when allocate a new CFID_arr entry, z_flag and z_urf_index need to copy from cfid_arr[cfio.back()]
 * e.g.
 *  SUB R1, R2, R3
 *  BZ R0, #4
 *  BNZ R0, #16
 */
struct CFID_base{
  int valid;
  int z_flag; // z_flag from last arithmetic instrn, initial to VALID
  int z_flag_valid; // initial to VALID, when an arithmetic instrn dispatch, set to INVALID; when this instrn finished EX, set to VALID
  int z_urf_index; // record the rd_tag of last arithmetic instrn, used for set z_flag_valid to VALID
  int rat_bak[NUM_REGS];
  // int urf_bak[NUM_UREGS];
  int urf_valid_bak[NUM_UREGS];
};

/* Model of APEX CPU */
struct APEX_CPU
{
  int clock;  /* Clock cycles elasped */
  int pc; /* Current program counter */

  // TODO: need to remove it
  uint32_t flags; /* 32 bits of flags, flags&0x1 is zero flag */

  /* TODO: could be removed */
  int regs[NUM_REGS]; /* Integer register file */
  int regs_valid[NUM_REGS];

  int rat[NUM_REGS]; /* register alias table */
  int r_rat[NUM_REGS]; /* retired rat */

  /* unify regster file */
  int urf[NUM_UREGS];
  int urf_valid[NUM_UREGS];  // function like free list,
                              // initial to VALID,
                              // when a instrn dispatch(rd_tag), set it INVALID, avoid reallocate;
                              // when a instrn commit, R_RAT point to it, don't change it
                              // when another instrn with the same rd commited, set it to VALID
  int urf_z_flag[NUM_UREGS]; // z_flag process is headle by CFIDs and CFIO
  // int urf_z_flag_valid[NUM_UREGS]; // initial to VALID,
                                   // when a arithmetic instrn dispatch(rd_tag), set to INVALID,
                                   // and set associated CFID entry.z_urf_index, tell BZ/BNZ to wait;
                                   // when this arithmetic instrn commited, set to VALID, telling

  /* fetch, DRD, IQ, ROB, intFU, mulFU */
  CPU_Stage stage[NUM_STAGES]; /* Array of 5 CPU_stage */
  Fetch_t fetch_stage;
  DRD_t drd;
  IQ_t iq;
  IntFU_t intFU;
  MulFU_t mulFU;
  LSQ_t lsq;
  MEM_t mem;
  ROB_t rob;

  Broadcast_t broadcast;

  CFID_base cfid_arr[NUM_CFID]; // cpu->cfid_arr, entry->cfid is true cfid
  std::deque<int> cfio; // control Flow instruction order, using cfio.back() as last_CFID, and its size should alway greater than 0

  int code_memory_size;
  APEX_Instruction* code_memory; /* Code Memory where instructions are stored */
  int data_memory[DATA_MEM_SIZE]; /* Data Memory */
  int ins_completed; /* Some stats */

};


/* basic functions */
int setStagetoNOPE(CPU_Stage* stage); // old
int setStagetoNOP(CPU_Stage_base* entry);
int copyStagetoNext(APEX_CPU* cpu, int stage_num, int index=UNUSED_INDEX);
int fetchValue(APEX_CPU* cpu, CPU_Stage_base* entry);
int URF_getValidIndex(APEX_CPU* cpu);
int CFID_getValidEntry(APEX_CPU* cpu);
int setZFlaginCFID_arr(APEX_CPU* cpu, CPU_Stage_base* entry);

/* print/debug functions */
void print_instruction(CPU_Stage* stage);
void print_stage_content(char* name, CPU_Stage* stage);
void print_all_stage(CPU_Stage* stages); /* old */
void print_all_stage(APEX_CPU* cpu);
void print_regs(int* regs, int* regs_valid);
void print_regs(APEX_CPU* cpu);
void print_data_memory(int* data_memory);
void print_flag_reg(uint32_t* flag_reg);

int Fetch_stage_init(Fetch_t* stage);
int DRD_init(DRD_t* stage);
int IQ_init(IQ_t *stage);
int IntFU_init(IntFU_t* stage);
int MulFU_init(MulFU_t* stage);
int LSQ_init(LSQ_t* stage);
int MEM_init(MEM_t* stage);
int ROB_init(ROB_t* stage);
int CFID_init(APEX_CPU* cpu);

int flush_restore(APEX_CPU* cpu, int cfid);

#endif /* CPU_BASE_H */
