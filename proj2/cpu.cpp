/*
 *  cpu.c
 *  Contains APEX cpu pipeline implementation
 */
#include "cpu.h"
#include "cpu_base.h"
#include "cpu_helper.h"
#include "file_parser.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define NDEBUG
#include <assert.h>

/* cpu.c private functions */
int takeCommand(APEX_CPU* cpu);
int Fetch_run(APEX_CPU* cpu);
int DRD_run(APEX_CPU* cpu);
int IQ_run(APEX_CPU* cpu);
int intFU_run(APEX_CPU* cpu);
int mulFU_run(APEX_CPU* cpu);
int LSQ_run(APEX_CPU* cpu);
int MEM_run(APEX_CPU* cpu);
int ROB_run(APEX_CPU* cpu);

/*
 * This function creates and initializes APEX cpu.
 */
APEX_CPU*
APEX_cpu_init(const char* filename)
{
  if (!filename) {
    fprintf(stderr, "filename invalid\n");
    return NULL;
  }

  APEX_CPU* cpu = (APEX_CPU*)malloc(sizeof(*cpu));
  if (!cpu) {
    fprintf(stderr, "allocate cpu space error\n");
    return NULL;
  }

  /* Initialize PC, Registers and all pipeline stages */
  cpu->clock = 0;
  cpu->pc = PC_START_INDEX;
  // cpu->flags = 0x0;

  /* REGs */
  // TODO_3: initial it to VALID, hope clients don't read garbadge from it
  memset(cpu->regs, 0x00, sizeof(int) * NUM_REGS);
  for(int i=0; i<NUM_REGS; ++i) cpu->regs_valid[i] = VALID;

  /* RAT and R_RAT */
  memset(cpu->rat, 0xFF, sizeof(int)*NUM_REGS); // set to -1
  memset(cpu->r_rat, 0xFF, sizeof(int)*NUM_REGS); // set to -1

  /* URF */
  memset(cpu->urf, 0x00, sizeof(int)*NUM_UREGS);
  for(int i=0; i<NUM_UREGS; ++i){ cpu->urf_z_flag[i]= true; cpu->urf_valid[i]= VALID; }

  /* old stages */
  memset(cpu->stage, 0x00, sizeof(CPU_Stage) * NUM_STAGES);

  /* new stages init */
  Fetch_stage_init(&cpu->fetch_stage);
  DRD_init(&cpu->drd);
  IQ_init(&cpu->iq);
  IntFU_init(&cpu->intFU);
  MulFU_init(&cpu->mulFU);
  // LSQ_init(&cpu->lsq);
  // MEM_init(&cpu->MEM);
  ROB_init(&cpu->rob);

  memset(cpu->data_memory, 0xFF, sizeof(int) * DATA_MEM_SIZE); // set to -1 for debug purpose
  cpu->ins_completed=0;

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);
  if (!cpu->code_memory) {
    fprintf(stderr, "create_code_memory error\n");
    free(cpu);
    return NULL;
  }

  if (ENABLE_DEBUG_MESSAGES) {
    fprintf(stdout,
            "APEX_CPU : Initialized APEX CPU, loaded %d instructions\n",
            cpu->code_memory_size);
    // fprintf(stderr, "APEX_CPU : Printing Code Memory\n");
    fprintf(stdout, "APEX_CPU : Printing Code Memory\n");
    printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode", "rd", "rs1", "rs2", "imm");

    for (int i = 0; i < cpu->code_memory_size; ++i) {
      printf("%-9s %-9d %-9d %-9d %-9d\n",
             cpu->code_memory[i].opcode,
             cpu->code_memory[i].rd,
             cpu->code_memory[i].rs1,
             cpu->code_memory[i].rs2,
             cpu->code_memory[i].imm);
    }
  }

  return cpu;
}

/*
 * This function de-allocates APEX cpu.
 */
void
APEX_cpu_stop(APEX_CPU* cpu)
{
  free(cpu->code_memory);
  free(cpu);
}

/*
 *  APEX CPU simulation loop
 */
int
APEX_cpu_run(APEX_CPU* cpu)
{
  while (1) {

    /* All the instructions committed, so exit */
    // TODO_2: is there a better condition??
    if (
        (
        cpu->pc >= get_pc(cpu->code_memory_size) &&
        !cpu->rob.entry.empty() &&
        cpu->rob.entry.front().pc >= get_pc(cpu->code_memory_size)
        )
        ||
        (
        !cpu->rob.entry.empty() &&
        strcmp(cpu->rob.entry.front().opcode, "HALT")==0
        )
      ){
      printf("(apex) >> Simulation Complete");
      break;
    }

    cpu->clock++;
    ROB_run(cpu);
    intFU_run(cpu);
    IQ_run(cpu);
    DRD_run(cpu);
    Fetch_run(cpu);
    // mulFU_run(cpu);
    // LSQ_run(cpu);
    // MEM_run(cpu);

    if (ENABLE_DEBUG_MESSAGES) {
      printf("================================================================================\n");
      printf("after Clock Cycle #: %d\n", cpu->clock);
      printf("================================================================================\n");
      print_all_stage(cpu);
    }


    if(enable_interactive){
      if(takeCommand(cpu)==-1){
        break;
      }
    }

  }

  return 0;
}


/*
 *  Fetch Stage of APEX Pipeline
 */
int
Fetch_run(APEX_CPU* cpu)
{
  int stage_num = F;
  Fetch_t* stage = &cpu->fetch_stage;
  stage->stalled = UNSTALLED;

  /* if it's new data, start busy_clock from begining */
  if(stage->busy == BUSY_NEW){
    stage->busy = BUSY_DEFAULT;
  }

  /* do the job, nothing for Fetch stage */
  if(stage->busy > BUSY_DONE){
    stage->busy--;
  }

  int isForwarded = FAILED;
  /* it's done, try to copy data to nextStage */
  if(stage->busy==BUSY_DONE){
    isForwarded = copyStagetoNext(cpu, stage_num);
    /* set nextStage to BUSY_NEW, next circle nextStage could start a new clock */
    if(isForwarded == SUCCEED)
      cpu->drd.busy = BUSY_NEW;
  }
  /* if it's initial, don't need to copy to next */
  else if(stage->busy == BUSY_INITIAL){
    isForwarded = SUCCEED;
  }
  /* business hasn't been done, do it in next circle */
  else{
    return 0;
  }

  /* forward failed */
  if(isForwarded == FAILED){
    stage->stalled = STALLED;
  }
  /* forward success or initial, fetch instrn from code_memory */
  else{
    /* fetch next work from code_memory */
    stage->busy = BUSY_NEW;
    stage->entry.pc = cpu->pc;
    cpu->pc += BYTES_PER_INS; // Update cpu->pc so it always point to unfetch instrn

    if(stage->entry.pc >= get_pc(cpu->code_memory_size)){
      setStagetoNOP(&stage->entry);
      stage->entry.pc = get_pc(cpu->code_memory_size); // because setStagetoNOPE set stage->pc to zero
    }
    else {
      APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(stage->entry.pc)];
      strcpy(stage->entry.opcode, current_ins->opcode);
      stage->entry.rd = current_ins->rd;
      stage->entry.rs1 = current_ins->rs1;
      stage->entry.rs2 = current_ins->rs2;
      stage->entry.imm = current_ins->imm;
    }
  }
  return 0;
}

/*
 *  Decode/Renaming/Dispatch Stage of APEX Pipeline
 */
int
DRD_run(APEX_CPU* cpu)
{
  int stage_num = DRD;
  DRD_t* stage = &cpu->drd;
  stage->stalled = UNSTALLED;

  /* only set by last stage, if it's new business, start a new busy clock */
  if(stage->busy == BUSY_NEW){
    stage->busy = BUSY_DEFAULT;
  }

  /* do the job */
  if(stage->busy > BUSY_DONE){
    /* fetch */
    fetchValue(cpu, &stage->entry);
    /* renaming, set up new most recently tag */
    int urf_index = URF_getValidIndex(cpu);
    if(urf_index == FAILED){
      stage->stalled = STALLED;
      return 0;
    }
    cpu->rat[stage->entry.rd] = urf_index;
    cpu->urf_valid[urf_index] = INVALID;
    stage->entry.rd_tag = urf_index;
    /* finish job */
    stage->busy--;
  }

  int isForwarded = FAILED;
  /* it's done, try to copy data to nextStage */
  if(stage->busy==BUSY_DONE){
    isForwarded = copyStagetoNext(cpu, stage_num);
  }
  /* business hasn't been done, do it in next circle */
  else{
    return 0;
  }

  /* forward failed */
  if(isForwarded == FAILED){
    stage->stalled = STALLED;
  }
  /* forward success, wait last stage to push new data */
  else{
    // stage->busy == BUSY_DONE check
    // stage->stalled = UNSTALLED check
    // last stage could forward new data and set BUSY_NEW flag
  }
  return 0;
}

/*
 * IQ stage don't need to rely on busy and stalled flag,
 * DRD would understand status of IQ by asking for valid entry
 */
int
IQ_run(APEX_CPU* cpu){
  IQ_t* stage = &cpu->iq;
  // stage->stalled = UNSTALLED;

  /* fetch value for source register, and set readyforIssue flag */
  for(int i{0}; i<NUM_IQ_ENTRY; ++i){
    if(stage->entry[i].valid == INVALID)
      fetchValue(cpu, &stage->entry[i]);
  }

  /* check readyforIssue flag, and select oldest issuable entry */
  int IQ_index_tointFU{UNUSED_INDEX};
  int IQ_index_tomulFU{UNUSED_INDEX};
  for(int i{0}; i<NUM_IQ_ENTRY; ++i){
    if(stage->entry[i].readyforIssue == VALID){
      /* mul go to mulFU */
      if(strcmp(stage->entry[i].opcode, "MUL")==0){
        if(IQ_index_tomulFU == UNUSED_INDEX ||
          stage->entry[i].dispatch_cycle < stage->entry[IQ_index_tomulFU].dispatch_cycle
          ){
          IQ_index_tomulFU = i;
        }
      }
      /* other instrn go to intFU */
      else {
        if(IQ_index_tointFU == UNUSED_INDEX ||
          stage->entry[i].dispatch_cycle < stage->entry[IQ_index_tointFU].dispatch_cycle
          ){
          IQ_index_tointFU = i;
        }
      }
    }
  }

  /* copy to intFU and mulFU */
  if(IQ_index_tointFU != UNUSED_INDEX){
    if(copyStagetoNext(cpu, IQ, IQ_index_tointFU) != FAILED){
      stage->entry[IQ_index_tointFU].valid = VALID;
    }
  }
  if(IQ_index_tomulFU != UNUSED_INDEX){
    if(copyStagetoNext(cpu, IQ, IQ_index_tomulFU) != FAILED){
      stage->entry[IQ_index_tomulFU].valid = VALID;
    }
  }

  return 0;
}

/*
 *  Execute Stage of APEX Pipeline
 */
int
intFU_run(APEX_CPU* cpu)
{
  int stage_num = intFU;
  IntFU_t* stage = &cpu->intFU;
  CPU_Stage_base* entry = &stage->entry;
  stage->stalled = UNSTALLED;

  /* only set by last stage, if it's new business, start a new busy clock */
  if(stage->busy == BUSY_NEW){
    stage->busy = BUSY_DEFAULT;
  }

  /* do the job */
  if(stage->busy > BUSY_DONE){
    // stage->entry.opcode
    if (strcmp(entry->opcode, "STORE") == 0 ||
        strcmp(entry->opcode, "LOAD") == 0
        ) {
      entry->mem_address = entry->rs2_value + entry->imm; // TODO_3: assume it doesn't excess the boundary
      entry->mem_address_valid = VALID;
    }

    else if(strcmp(entry->opcode, "MOVC")==0){
      entry->buffer = entry->imm;
      entry->buffer_valid = VALID;
    }
    else if (strcmp(entry->opcode, "ADD") == 0) {
      entry->buffer = entry->rs1_value + entry->rs2_value;
      entry->buffer_valid = VALID;
      // if(entry->buffer == 0) cpu->flags |= 0x1;
      // else cpu->flags &= ~0x1;
    }
    else if (strcmp(entry->opcode, "SUB") == 0) {
      entry->buffer = entry->rs1_value - entry->rs2_value;
      entry->buffer_valid = VALID;
      // if(stage->buffer == 0) cpu->flags |= 0x1;
      // else cpu->flags &= ~0x1;
    }
    else if (strcmp(entry->opcode, "MUL") == 0) {
      fprintf(stderr, "have MUL instrn in intFU stage\n");
      assert(0);
    }
    else if (strcmp(entry->opcode, "AND") == 0) {
      entry->buffer = entry->rs1_value & entry->rs2_value;
      entry->buffer_valid = VALID;
    }
    else if (strcmp(entry->opcode, "OR") == 0) {
      entry->buffer = entry->rs1_value | entry->rs2_value;
      entry->buffer_valid = VALID;
    }
    else if (strcmp(entry->opcode, "EX-OR") == 0) {
      entry->buffer = entry->rs1_value ^ entry->rs2_value;
      entry->buffer_valid = VALID;
    }
    else if(strcmp(entry->opcode, "JUMP")==0 ||
        strcmp(entry->opcode, "JMP") ==0
        ){
      /*
      stage->buffer= stage->rs1_value + stage->imm;
      cpu->pc = stage->pc + stage->buffer;
      if(cpu->pc < PC_START_INDEX ) cpu->pc = PC_START_INDEX;
      if(cpu->pc > get_pc(cpu->code_memory_size)) cpu->pc = get_pc(cpu->code_memory_size);
      setStagetoNOPE(&cpu->stage[F]);
      setStagetoNOPE(&cpu->stage[DRF]);
      */
    }
    else if(strcmp(entry->opcode, "JAL")==0){
      // TODO
    }

    else if(strcmp(entry->opcode, "BZ")==0){
      /*
      if((cpu->flags&0x1) == 0x1){
        cpu->pc = stage->pc + stage->imm;
        if(cpu->pc < PC_START_INDEX ) cpu->pc = PC_START_INDEX;
        if(cpu->pc > get_pc(cpu->code_memory_size)) cpu->pc = get_pc(cpu->code_memory_size);
        setStagetoNOPE(&cpu->stage[F]);
        setStagetoNOPE(&cpu->stage[DRF]);
      }
      */
    }
    else if(strcmp(entry->opcode, "BNZ")==0){
      /*
      if((cpu->flags&0x1) == 0x0){
        cpu->pc = stage->pc + stage->imm;
        if(cpu->pc < PC_START_INDEX ) cpu->pc = PC_START_INDEX;
        if(cpu->pc > get_pc(cpu->code_memory_size)) cpu->pc = get_pc(cpu->code_memory_size);
        setStagetoNOPE(&cpu->stage[F]);
        setStagetoNOPE(&cpu->stage[DRF]);
      }
      */
    }
    else{
      // NOP, unknown instruction
    }
    /* finish job */
    stage->busy--;
  }

  int isForwarded = FAILED;
  /* it's done, try to copy data to nextStage */
  if(stage->busy==BUSY_DONE){
    // TODO: intFU->ROB
    isForwarded = copyStagetoNext(cpu, stage_num);
  }
  // TODO_2: it seems no reason that intFU will stall for ROB is stalled or busy
  // besides it is always one circle delay
  /* business hasn't been done, do it in next circle */
  else{
    return 0;
  }

  /* forward failed */
  if(isForwarded == FAILED){
    stage->stalled = STALLED;
  }
  /* forward success, wait last stage to push new data */
  else{
    // stage->busy == BUSY_DONE check
    // stage->stalled = UNSTALLED check
    // last stage could forward new data and set BUSY_NEW flag
  }
  return 0;
}

int
mulFU_run(APEX_CPU* cpu){
  int stage_num = intFU;
  MulFU_t* stage = &cpu->mulFU;
  CPU_Stage_base* entry = &stage->entry;
  stage->stalled = UNSTALLED;

  /* only set by last stage, if it's new business, start a new busy clock */
  if(stage->busy == BUSY_NEW){
    stage->busy = BUSY_MUL_DELAY;
  }

  /* do the job */
  if(stage->busy > BUSY_DONE){
    if(strcmp(entry->opcode, "MUL")==0){
      entry->buffer = entry->rs1_value * entry->rs2_value;
      // if(stage->buffer == 0) cpu->flags |= 0x1;
      // else cpu->flags &= ~0x1;
    }
    else{
      fprintf(stderr, "got other instrn in mulFU stage\n");
      assert(0);
    }
    /* finish job */
    stage->busy--;
  }

  int isForwarded = FAILED;
  /* it's done, try to copy data to nextStage */
  if(stage->busy==BUSY_DONE){
    // TODO: mulFU->ROB
    isForwarded = copyStagetoNext(cpu, stage_num);
  }
  // TODO_2: it seems no reason that intFU will stall for ROB is stalled or busy
  // besides it is always one circle delay
  /* business hasn't been done, do it in next circle
   * or this is BUSY_INITIAL, don't need to do process
   */
  else{
    return 0;
  }

  /* forward failed */
  if(isForwarded == FAILED){
    stage->stalled = STALLED;
  }
  /* forward success, wait last stage to push new data */
  else{
    // stage->busy == BUSY_DONE check
    // stage->stalled = UNSTALLED check
    // last stage could forward new data and set BUSY_NEW flag
  }
  return 0;
}

int
LSQ_run(APEX_CPU* cpu){
  // TODO:
  cpu++;
  return 0;
}

/*
 *  Memory Stage of APEX Pipeline
 */
int
MEM_run(APEX_CPU* cpu)
{
  // TODO:
  cpu++;
  return 0;
}

/*
 * the delay module doesn't apply to ROB, do the work directly
 */
int
ROB_run(APEX_CPU* cpu){
  std::deque<CPU_Stage_base>* entry = &cpu->rob.entry;
  while(entry->size()>0){
    auto& it = entry->front();
    if(it.completed==VALID){
      /* instrns with Rd */
      if(it.rd != UNUSED_REG_INDEX){
        /* retire old commited value */
        if(cpu->r_rat[it.rd] != UNUSED_REG_INDEX){
          int old_urf_index = cpu->r_rat[it.rd];
          cpu->urf_valid[old_urf_index] = VALID;
        }
        /* set up new commited value, e.g. JAL */
        cpu->r_rat[it.rd] = it.rd_tag;
        assert(it.buffer_valid == VALID);
        cpu->urf[it.rd_tag] = it.buffer;
      }
      /* STORE, BZ, BNZ, JMP, HALT, NOP */
      else{
      }
      entry->pop_front();
    }
    /* head is not completed */
    else{
      break;
    }
  }
  return 0;
}

int
takeCommand(APEX_CPU* cpu){
  static uint counter=0;
  if(counter!=0){
    counter--;
    return 0;
  }

COMMAND:
  printf("\ntypein command: initialize(r), simulate(sim) <n>, display(p), quit(q)\n");
  printf("(apex) >>");

  char command[64];
  scanf("%s", command);
  if(strcmp(command, "quit")==0 ||
      strcmp(command, "q")==0
    ){
    return -1;
  }

  else if(strcmp(command, "initialize")==0 ||
      strcmp(command, "init")==0 ||
      strcmp(command, "r")==0
    ){
    cpu->clock=0;
    cpu->pc = PC_START_INDEX;
    memset(cpu->regs, 0, NUM_REGS*sizeof(cpu->regs[0]));
    memset(cpu->regs_valid, VALID, NUM_REGS*sizeof(cpu->regs_valid[0]));
    memset(cpu->stage, 0, NUM_STAGES*sizeof(cpu->stage[0]));
    memset(cpu->data_memory, 0, sizeof(int) * DATA_MEM_SIZE);
    cpu->ins_completed=0;
    printf("initial complete.\n");
    return 0;
  }

  else if(strcmp(command, "simulate")==0 ||
      strcmp(command, "sim")==0 ||
      strcmp(command, "s")==0
      ){
    scanf("%d", &counter);
    return 0;
  }

  else if(strcmp(command, "display")==0 ||
      strcmp(command, "p")==0
      ){
    print_all_stage(cpu->stage);
    print_flag_reg(&cpu->flags);
    print_regs(cpu->regs, cpu->regs_valid);
    print_data_memory(cpu->data_memory);
    goto COMMAND;
  }

  else if(strcmp(command, "next")==0 ||
      strcmp(command, "n")==0
      ){
    // nothing to do
    return 0;
  }

  else{
    printf("unknow command\n");
  }
  return 0;
}
