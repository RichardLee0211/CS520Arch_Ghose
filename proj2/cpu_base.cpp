#include "cpu_base.h"
#include "global.h"

#include<string.h>
#include<stdio.h>
#include<limits.h>

/* old */
void
print_instruction(CPU_Stage* stage)
{
  if (strcmp(stage->opcode, "STORE") == 0 ) {
    printf(
        "%s,R%d[%04d],R%d[%04d],#%d ", stage->opcode,
        stage->rs1, stage->rs1_value,
        stage->rs2, stage->rs2_value,
        stage->imm);
  }
  else if(strcmp(stage->opcode, "LOAD") == 0){
    printf(
        "%s,R%d[%04d],R%d[%04d],#%d ", stage->opcode,
        stage->rd, stage->buffer,
        stage->rs2, stage->rs2_value,
        stage->imm);
  }

  else if (strcmp(stage->opcode, "MOVC") == 0) {
    printf("%s,R%d[%04d],#%d ", stage->opcode, stage->rd, stage->buffer, stage->imm);
  }

  else if (strcmp(stage->opcode, "ADD") == 0 ||
      strcmp(stage->opcode, "SUB") == 0 ||
      strcmp(stage->opcode, "AND") == 0 ||
      strcmp(stage->opcode, "OR") == 0 ||
      strcmp(stage->opcode, "EX-OR") == 0 ||
      strcmp(stage->opcode, "MUL") == 0
      ){
    printf("%s,R%d[%04d],R%d[%04d],R%d[%04d] ", stage->opcode,
        stage->rd, stage->buffer,
        stage->rs1, stage->rs1_value, stage->rs2, stage->rs2_value);
  }

  else if(strcmp(stage->opcode, "PRINT_REG")==0){
    printf("%s,R%d ", stage->opcode, stage->rs1);
  }
  else if(strcmp(stage->opcode, "PRINT_MEM")==0 ||
      strcmp(stage->opcode, "JUMP")==0 ||
      strcmp(stage->opcode, "JMP")==0
      ){
    printf("%s,R%d[%04d],#%d ", stage->opcode, stage->rs1, stage->rs1_value, stage->imm);
  }
  else if(strcmp(stage->opcode, "BZ")==0 ||
      strcmp(stage->opcode, "BNZ")==0
      ){
    printf("%s,#%d ", stage->opcode, stage->imm);
  }

  else if(strcmp(stage->opcode, "NOP") == 0 ||
      strcmp(stage->opcode, "HALT")==0
      ){
    printf("%s ", stage->opcode);
  }

  else{
    printf("%s ", "UNKNOWN");
  }

  printf(" %s", stage->stalled ? "STALLED":"UNSTALLED");
}

/* new */
void
print_instruction(CPU_Stage_base* stage)
{
  if (strcmp(stage->opcode, "STORE") == 0 ) {
    printf(
        "%s,R%d[U%02d %02d],R%d[U%02d %02d],#%d ", stage->opcode,
        stage->rs1, stage->rs1_tag, stage->rs1_value,
        stage->rs2, stage->rs2_tag, stage->rs2_value,
        stage->imm);
  }
  else if(strcmp(stage->opcode, "LOAD") == 0){
    printf(
        "%s,R%d[U%02d %02d],R%d[U%02d %02d],#%d ", stage->opcode,
        stage->rd, stage->rd_tag, stage->buffer,
        stage->rs2, stage->rs2_tag, stage->rs2_value,
        stage->imm);
  }

  else if (strcmp(stage->opcode, "MOVC") == 0) {
    printf("%s,R%d[U%02d %02d],#%d ", stage->opcode,
        stage->rd, stage->rd_tag, stage->buffer, stage->imm);
  }

  else if (strcmp(stage->opcode, "ADD") == 0 ||
      strcmp(stage->opcode, "SUB") == 0 ||
      strcmp(stage->opcode, "AND") == 0 ||
      strcmp(stage->opcode, "OR") == 0 ||
      strcmp(stage->opcode, "EX-OR") == 0 ||
      strcmp(stage->opcode, "MUL") == 0
      ){
    printf("%s,R%d[U%02d %02d],R%d[U%02d %02d],R%d[U%02d %02d] ", stage->opcode,
        stage->rd, stage->rd_tag, stage->buffer,
        stage->rs1, stage->rs1_tag, stage->rs1_value,
        stage->rs2, stage->rs2_tag, stage->rs2_value);
  }

  else if(
      strcmp(stage->opcode, "JUMP")==0 ||
      strcmp(stage->opcode, "JMP")==0
      ){
    printf("%s,R%d[U%02d %02d],#%d ", stage->opcode,
        stage->rs1, stage->rs1_tag, stage->rs1_value,
        stage->imm);
  }
  else if(strcmp(stage->opcode, "BZ")==0 ||
      strcmp(stage->opcode, "BNZ")==0
      ){
    printf("%s,#%d ", stage->opcode, stage->imm);
  }

  else if(strcmp(stage->opcode, "NOP") == 0 ||
      strcmp(stage->opcode, "HALT")==0
      ){
    printf("%s ", stage->opcode);
  }

  else{
    printf("%s ", "UNKNOWN");
  }
}

void print_Fetch_stage(Fetch_t* stage){
  printf("%-15s: pc(%04d) ", "Fetch", stage->entry.pc);
  print_instruction(&stage->entry);
  printf(" %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

void print_DRD(DRD_t* stage){
  printf("%-15s: pc(%04d) ", "DRD", stage->entry.pc);
  print_instruction(&stage->entry);
  printf(" %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

void print_IQ(IQ_t* stage){
  for(int i{0}; i<NUM_IQ_ENTRY; ++i){
    if(stage->entry[i].valid == INVALID){
      printf("%-15s[%02d]: pc(%04d) ", "IQ", i, stage->entry[i].pc);
      print_instruction(&stage->entry[i]);
    }
  }
}

void print_ROB(ROB_t* stage){
  for(auto i: stage->entry){
    printf("%-15s: pc(%04d) ", "ROB",  i.pc);
    // TODO: ??
    print_instruction(&i);

  }
}

void print_intFU(IntFU_t* stage){
  printf("%-15s: pc(%04d) ", "DRD", stage->entry.pc);
  print_instruction(&stage->entry);
  printf(" %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

void print_mulFU(MulFU_t* stage){
  printf("%-15s: pc(%04d) ", "DRD", stage->entry.pc);
  print_instruction(&stage->entry);
  printf(" %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

/*
 * old
 * Debug function which dumps the cpu stage content
 */
void
print_stage_content(const char* name, CPU_Stage* stage)
{
  printf("%-15s: pc(%04d) ", name, stage->pc);
  print_instruction(stage);
  printf("\n");
}


/* old */
void print_all_stage(CPU_Stage* stages){
  print_stage_content("Writeback", &stages[WB]);
  print_stage_content("Memory", &stages[MEM]);
  print_stage_content("Execute", &stages[EX]);
  print_stage_content("Decode/RF", &stages[DRF]);
  print_stage_content("Fetch", &stages[F]);
}

/* new */
void print_all_stage(APEX_CPU* cpu){
  // print_ROB(&cpu->rob);
  // print_intFU(&cpu->intFU);
  print_IQ(&cpu->iq);
  print_DRD(&cpu->drd);
  print_Fetch_stage(&cpu->fetch_stage);
}

/* old */
void print_regs(int* regs, int* regs_valid){
  printf("REGS: \n");
  for(int i=1; i<NUM_REGS; ++i){
    printf("%3d(%04d)  ", regs[i], regs_valid[i]);
    if((i%(NUM_REGS/4))==0) printf("\n");
  }
  printf("\n");
}

/* new */
void print_regs(APEX_CPU* cpu){
  for(int i{0}; i<NUM_REGS; ++i){
    int u_index = cpu->rat[i];
    printf("%03d  ", cpu->urf[u_index]);
    if((i%(NUM_REGS/4))==0) printf("\n");
  }
  printf("\n");
}

void print_data_memory(int* data_memory){
  for(int i=0; i<100; ++i){
    if(i%8==0) printf("\nDATA_MEM[%02d]", i);
    printf("%3d  ", data_memory[i]);
  }
  printf("\n");
}

/*
 * print in invert order
 */
void print_flag_reg(uint32_t* flag_reg){
  printf("FLAGS: ");
  for(int i=0; i<32; ++i){
    if(i%4==0) printf(" ");
    if((*flag_reg>>i)%2 == 0){
      printf("0");
    }else{
      printf("1");
    }
  }
  printf("\n");
}

/* functions on stages */
/* init functions */
int stage_base_init(CPU_Stage_base* entry){
  // set all most everything to -1
  memset(entry, 0xFF, sizeof(*entry));
  memset(entry->opcode, 0x00, OPCODE_SIZE*sizeof(char));
  entry->imm = UNUSED_IMM;
  return 0;
}

int Fetch_stage_init(Fetch_t* stage){
  stage_base_init(&stage->entry);
  stage->busy = BUSY_DONE;
  stage->stalled = UNSTALLED;
  return 0;
}

int DRD_init(DRD_t* stage){
  // stage_base_init(&stage->latch);
  stage_base_init(&stage->entry);
  stage->busy = BUSY_DONE;
  stage->stalled = UNSTALLED;
  return 0;
}

int IQ_init(IQ_t *stage){
  for(int i{0}; i<NUM_IQ_ENTRY; ++i) { stage_base_init(&stage->entry[i]); }
  stage->busy = BUSY_DONE;
  stage->stalled = UNSTALLED;
  return 0;
}

int ROB_init(ROB_t* stage){
  // stage->entry; // std::deque will take care of it
  stage->busy = BUSY_DONE;
  stage->stalled = UNSTALLED;
  return 0;
}

int IntFU_init(IntFU_t* stage){
  // stage_base_init(&stage->latch);
  stage_base_init(&stage->entry);
  stage->busy = BUSY_DONE;
  stage->stalled = UNSTALLED;
  return 0;
}

/* have some basic function here */
/* old */
int setStagetoNOPE(CPU_Stage* stage){
  stage->pc=0;
  strcpy(stage->opcode, "NOP");
  stage->rs1 = UNUSED_REG_INDEX;
  stage->rs2 = UNUSED_REG_INDEX;
  stage->rd = UNUSED_REG_INDEX;
  stage->imm = UNUSED_IMM;
  return 0;
}

/* new */
int setStagetoNOP(CPU_Stage_base* entry){
  entry->pc=0;
  strcpy(entry->opcode, "NOP");
  entry->rs1 = UNUSED_REG_INDEX;
  entry->rs2 = UNUSED_REG_INDEX;
  entry->rd = UNUSED_REG_INDEX;
  entry->imm = UNUSED_IMM;
  return 0;
}

/*
 * fetch data for rs1 and rs2 from intFU, mulFU, mem, ROB
 * work for DRD or IQ stage
 * and set readyforIssue flags when it got all src value
 */
int fetchValue(APEX_CPU* cpu, CPU_Stage_base* entry){
  int failed = INVALID;
  /* fetch rs1_value if its needed and haven't been fetched */
  if(entry->rs1 != UNUSED_REG_INDEX && entry->rs1_value_valid==INVALID){
    /* because rat entries initial to UNUSED_REG_INDEX */
    if(cpu->rat[entry->rs1] == UNUSED_REG_INDEX){
      fprintf(stderr, "using un-initial register\n");
      assert(0);
    }
    entry->rs1_tag = cpu->rat[entry->rs1];
    /* lastest instance could in ROB and commited value */
    if(cpu->urf_valid[entry->rs1_tag] == VALID){
      entry->rs1_value = cpu->urf[entry->rs1_tag];
      entry->rs1_value_valid = VALID;
    }
    /* lastest instance could in intFU, and just finish calulation */
    else if(entry->rs1_tag == cpu->broadcast.tag_intFU){
      entry->rs1_value = cpu->broadcast.data_intFU;
      entry->rs1_value_valid = VALID;
    }
    /* lastest instance could in mulFU, and just finish calulation */
    else if(entry->rs1_tag == cpu->broadcast.tag_mulFU){
      entry->rs1_value = cpu->broadcast.data_mulFU;
      entry->rs1_value_valid = VALID;
    }
    /* lastest instance could in mem, and just finish calulation */
    else if(entry->rs1_tag == cpu->broadcast.tag_mem){
      entry->rs1_value = cpu->broadcast.data_mem;
      entry->rs1_value_valid = VALID;
    }
    /* lastest instance could in IQ, then don't copy data */
    else{
      failed = VALID;
    }
  }
  if(entry->rs2 != UNUSED_REG_INDEX && entry->rs2_value_valid==INVALID){
    /* because rat entries initial to UNUSED_REG_INDEX */
    if(cpu->rat[entry->rs2] == UNUSED_REG_INDEX){
      fprintf(stderr, "using un-initial register\n");
      assert(0);
    }
    entry->rs2_tag = cpu->rat[entry->rs2];
    /* lastest instance could in ROB and commited value */
    if(cpu->urf_valid[entry->rs2_tag] == VALID){
      entry->rs2_value = cpu->urf[entry->rs2_tag];
      entry->rs2_value_valid = VALID;
    }
    /* lastest instance could in intFU, and just finish calulation */
    else if(entry->rs2_tag == cpu->broadcast.tag_intFU){
      entry->rs2_value = cpu->broadcast.data_intFU;
      entry->rs2_value_valid = VALID;
    }
    /* lastest instance could in mulFU, and just finish calulation */
    else if(entry->rs2_tag == cpu->broadcast.tag_mulFU){
      entry->rs2_value = cpu->broadcast.data_mulFU;
      entry->rs2_value_valid = VALID;
    }
    /* lastest instance could in mem, and just finish calulation */
    else if(entry->rs2_tag == cpu->broadcast.tag_mem){
      entry->rs2_value = cpu->broadcast.data_mem;
      entry->rs2_value_valid = VALID;
    }
    /* lastest instance could in IQ, then don't copy data */
    else{
      failed = VALID;
    }
  }
  /* if got all rs_values or never need one, set readyforIssue */
  if(failed == INVALID){
    entry->readyforIssue = VALID;
  }
  return 0;
}

/*
int isEntryReadyforIssue(APEX_CPU*, CPU_Stage_base* entry){
  if(entry->rs1 != UNUSED_REG_INDEX){
  }
}
*/

int URF_getValidIndex(APEX_CPU* cpu){
  int i{0};
  for(; i<NUM_UREGS; ++i){
    if(cpu->urf_valid[i] == VALID){
      return i;
    }
  }
  return FAILED;
}

int IQ_getValidEntry(APEX_CPU* cpu){
  for(int i{0}; i< NUM_IQ_ENTRY; ++i){
    if(cpu->iq.entry[i].valid == VALID){
      return i;
    }
  }
  return FAILED;
}

int ROB_getValidEntry(APEX_CPU* cpu){
  if(cpu->rob.entry.size() < NUM_ROB_ENTRY)
    return 0;
  else
    return FAILED;
}

int LSQ_getValidEntry(APEX_CPU* cpu){
  if(cpu->lsq.entry.size() < NUM_LSQ_ENTRY)
    return 0;
  else
    return FAILED;
}

/*
 * usually isForwarded receive return value
 * 0 means success
 * FAILED means need to wait
 */
int copyStagetoNext(APEX_CPU* cpu, int stage_num, int index){
  if(!cpu || stage_num<F || stage_num >= NUM_STAGES){
    fprintf(stderr, "invalid cpu ptr or stage_num");
    assert(0);
  }
  /* F->DRD */
  if(stage_num == F){
    Fetch_t* stage = &cpu->fetch_stage;
    DRD_t* nextStage = &cpu->drd;
    if(nextStage->busy > BUSY_DONE || nextStage->stalled == STALLED){
      return FAILED;
    }
    nextStage->entry= stage->entry;
    return 0;
  }
  /* dispatch logic, DRD->... */
  else if(stage_num == DRD){
    DRD_t* stage = &cpu->drd;
    /* LOAD, STORE: DRD->IQ, DRD->ROB, DRD->LSQ */
    if(strcmp(stage->entry.opcode, "LOAD")==0 ||
        strcmp(stage->entry.opcode, "STORE")==0
      ){
      if( IQ_getValidEntry(cpu) != FAILED &&
          ROB_getValidEntry(cpu) != FAILED &&
          LSQ_getValidEntry(cpu) != FAILED
        ){
        // set dispatch_cycle;
        stage->entry.dispatch_cycle = cpu->clock;
        // copy to IQ, ROB and LSQ
        int IQ_index = IQ_getValidEntry(cpu);
        cpu->iq.entry[IQ_index] = stage->entry;
        cpu->iq.entry[IQ_index].valid = INVALID;
        cpu->rob.entry.push_back(stage->entry);
        cpu->lsq.entry.push_back(stage->entry);
      }
      else{
        return FAILED;
      }
    }
    // else if Branch instrn, need allocate CFID
    /* other instrns: DRD->IQ, DRD->ROB */
    else {
      if(IQ_getValidEntry(cpu) != FAILED &&
         ROB_getValidEntry(cpu) != FAILED
        ){
        // set dispatch_cycle;
        stage->entry.dispatch_cycle = cpu->clock;
        // copy to IQ and ROB;
        int IQ_index = IQ_getValidEntry(cpu);
        cpu->iq.entry[IQ_index] = stage->entry;
        cpu->rob.entry.push_back(stage->entry);
      }
      else{
        return FAILED;
      }
    }
  }
  /* IQ->intFU or IQ->mulFU */
  else if(stage_num == IQ){
    IQ_t* stage = &cpu->iq;
    CPU_Stage_base* entry = &stage->entry[index];
    /* mul instrn go to mulFU */
    if(strcmp(entry->opcode, "MUL")==0){
      MulFU_t* nextStage = &cpu->mulFU;
      if(nextStage->busy > BUSY_DONE || nextStage->stalled == STALLED){
        return FAILED;
      }
      nextStage->entry= *entry;
      return 0;
    }
    /* others go to intFU */
    else{
      IntFU_t* nextStage = &cpu->intFU;
      if(nextStage->busy > BUSY_DONE || nextStage->stalled == STALLED){
        return FAILED;
      }
      nextStage->entry= *entry;
      return 0;
    }
  }
  /* intFU->broadcast, intFU->ROB */
  else if(stage_num == intFU){
    /* set ROB entry buffer and valid bit */
  }
  /* mulFU->broadcast, mulFU->ROB */
  else if(stage_num == mulFU){

  }
  /* LSQ->MEM */
  else if(stage_num == LSQ){

  }
  /* MEM->ROB, MEM->broadcast */
  else if(stage_num == MEM ){
  }
  /* */
  else if(stage_num == ROB){
    // nothing..
  }

  return 0;
}
