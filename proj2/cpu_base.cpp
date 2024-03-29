#include "cpu_base.h"
#include "global.h"

#include<string.h>
#include<stdio.h>
#include<limits.h>
#include<assert.h>

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

  printf(" %s", stage->stalled ? "STALLED":"");
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
  else if(strcmp(stage->opcode, "ADDL")==0 ||
      strcmp(stage->opcode, "SUBL")==0
      ){
    printf("%s,R%d[U%02d %02d],R%d[U%02d %02d],#%d  ", stage->opcode,
        stage->rd, stage->rd_tag, stage->buffer,
        stage->rs1, stage->rs1_tag, stage->rs1_value,
        stage->imm);
  }

  else if(
      strcmp(stage->opcode, "JUMP")==0 ||
      strcmp(stage->opcode, "JMP")==0
      ){
    printf("%s,R%d[U%02d %02d],#%d ", stage->opcode,
        stage->rs1, stage->rs1_tag, stage->rs1_value,
        stage->imm);
  }
  else if(strcmp(stage->opcode, "JAL")==0){
    printf("%s,R%d[U%02d %02d],R%d[U%02d %02d],#%d ", stage->opcode,
        stage->rd, stage->rd, stage->buffer,
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
  printf("%-15s: pc(%04d,%03d,%1d) ", "Fetch",
      stage->entry.pc, stage->entry.dispatch_cycle, stage->entry.cfid);
  print_instruction(&stage->entry);
  printf(" %s %d\n", stage->stalled ? "STALLED": "", stage->busy);
}

void print_DRD(DRD_t* stage){
  printf("%-15s: pc(%04d,%03d,%1d) ", "DRD",
      stage->entry.pc, stage->entry.dispatch_cycle, stage->entry.cfid);
  print_instruction(&stage->entry);
  printf(" %s %d\n", stage->stalled ? "STALLED": "", stage->busy);
}

void print_IQ(IQ_t* stage){
  int empty = VALID;
  for(int i{0}; i<NUM_IQ_ENTRY; ++i){
    if(stage->entry[i].valid == INVALID){
      printf("%10s[%02d] : pc(%04d,%03d,%1d) ", "IQ", i,
          stage->entry[i].pc, stage->entry[i].dispatch_cycle, stage->entry[i].cfid);
      print_instruction(&stage->entry[i]);
      printf(" %s \n", stage->entry[i].readyforIssue==VALID ? "READY" : "");
      empty = INVALID;
    }
  }
  if(empty == VALID){
    printf("%15s: %s\n", "IQ ", "empty");
  }
}

void print_ROB(ROB_t* stage){
  if(stage->entry.size()==0){
    printf("%15s: %s\n", "ROB ", "empty");
    return;
  }
  for(auto& i: stage->entry){
    printf("%15s: pc(%04d,%03d,%1d) ", "ROB ",  i.pc, i.dispatch_cycle, i.cfid);
    print_instruction(&i);
    printf(" %s", i.completed==VALID ? "COMPLETE" : "");
    printf("\n");
  }
}

void print_intFU(IntFU_t* stage){
  /* busy is done, and data forward complete */
  if(stage->busy == BUSY_WAIT && stage->stalled == UNSTALLED){
    printf("%-15s: empty\n", "intFU");
  }
  else{
    printf("%-15s: pc(%04d,%03d,%1d) ", "intFU",
        stage->entry.pc, stage->entry.dispatch_cycle, stage->entry.cfid);
    print_instruction(&stage->entry);
    printf(" %s %d\n", stage->stalled ? "STALLED": "", stage->busy);
  }
}

void print_mulFU(MulFU_t* stage){
  /* busy is done, and data forward complete */
  if(stage->busy == BUSY_WAIT && stage->stalled == UNSTALLED){
    printf("%-15s: empty\n", "mulFU");
  }
  else{
    printf("%-15s: pc(%04d,%03d,%1d) ", "mulFU",
        stage->entry.pc, stage->entry.dispatch_cycle, stage->entry.cfid);
    print_instruction(&stage->entry);
    printf(" %s %d\n", stage->stalled ? "STALLED": "", stage->busy);
  }
}

void print_MEM(MEM_t* stage){
  /* busy is done, and data forward complete */
  if(stage->busy == BUSY_WAIT && stage->stalled == UNSTALLED){
    printf("%-15s: empty\n", "MEM");
  }
  else{
    printf("%-15s: pc(%04d,%03d,%1d) ", "MEM",
        stage->entry.pc, stage->entry.dispatch_cycle, stage->entry.cfid);
    print_instruction(&stage->entry);
    printf(" %s %d\n", stage->stalled ? "STALLED": "", stage->busy);
  }
}

void print_LSQ(LSQ_t* stage){
  if(stage->entry.size()==0){
    printf("%15s: %s\n", "LSQ ", "empty");
    return;
  }
  for(auto& i: stage->entry){
    printf("%15s: pc(%04d,%03d,%1d) ", "LSQ",  i.pc, i.dispatch_cycle, i.cfid);
    print_instruction(&i);
    printf(" %s\n", i.mem_address_valid == VALID ? "ADDR_VALID": "");
  }
}

void print_R_RAT(APEX_CPU* cpu){
  for(int i{0}; i<NUM_REGS; ++i){
    if(i%8==0) printf("R_RAT[%02d]: ", i);
    printf("%02d  ", cpu->r_rat[i]);
    if((i%8)==7) printf("\n");
  }
  printf("\n");
}

void print_RAT(APEX_CPU* cpu){
  for(int i{0}; i<NUM_REGS; ++i){
    if(i%8==0) printf("RAT[%02d]: ", i);
    printf("%02d  ", cpu->rat[i]);
    if((i%8)==7) printf("\n");
  }
  printf("\n");
}

void print_URF(APEX_CPU* cpu){
  for(int i=0; i<NUM_UREGS; ++i){
    if(i%10==0) printf("URF[%02d]: ", i);
    printf("%3d:%c  ", cpu->urf[i], cpu->urf_valid[i]==VALID ? 'V' : 'I');
    if(i%10==9) printf("\n");
  }
  printf("\n");
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
  print_ROB(&cpu->rob);
  printf("''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''\n");
  print_MEM(&cpu->mem);
  print_LSQ(&cpu->lsq);
  printf("''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''\n");
  print_intFU(&cpu->intFU);
  print_mulFU(&cpu->mulFU);
  print_IQ(&cpu->iq);
  printf("''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''\n");
  print_DRD(&cpu->drd);
  print_Fetch_stage(&cpu->fetch_stage);
  printf("''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''\n");
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
    if(i%8==0) printf("ARF[%02d]: ", i);
    int u_index = cpu->r_rat[i];
    if(u_index != UNUSED_REG_INDEX)
      printf("%03d  ", cpu->urf[u_index]);
    else
      printf("N/A  ");
    if((i%8)==7) printf("\n");
  }
  printf("\n");
}

void print_data_memory(int* data_memory){
  for(int i=0; i<NUM_DATA_MEM_PRINT; ++i){
    if(i%8==0) printf("DATA_MEM[%02d]: ", i);
    printf("%3d  ", data_memory[i]);
    if(i%8==7) printf("\n");
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
  entry->pc = 0;
  entry->imm = UNUSED_IMM;
  return 0;
}

int Fetch_stage_init(Fetch_t* stage){
  stage_base_init(&stage->entry);
  stage->busy = BUSY_INITIAL;
  stage->stalled = UNSTALLED;
  return 0;
}

int DRD_init(DRD_t* stage){
  stage_base_init(&stage->entry);
  stage->busy = BUSY_INITIAL;
  stage->stalled = UNSTALLED;
  return 0;
}

int IQ_init(IQ_t *stage){
  for(int i{0}; i<NUM_IQ_ENTRY; ++i) {
    stage_base_init(&stage->entry[i]);
    stage->entry[i].valid = VALID;
  }
  stage->busy = BUSY_INITIAL;
  stage->stalled = UNSTALLED;
  return 0;
}

int IntFU_init(IntFU_t* stage){
  // stage_base_init(&stage->latch);
  stage_base_init(&stage->entry);
  stage->busy = BUSY_INITIAL;
  stage->stalled = UNSTALLED;
  return 0;
}

int MulFU_init(MulFU_t* stage){
  stage_base_init(&stage->entry);
  stage->busy = BUSY_INITIAL;
  stage->stalled = UNSTALLED;
  return 0;
}

int ROB_init(ROB_t* stage){
  // stage->entry; // std::deque will take care of it
  stage->busy = BUSY_INITIAL;
  stage->stalled = UNSTALLED;
  return 0;
}

int MEM_init(MEM_t* stage){
  stage_base_init(&stage->entry);
  stage->busy = BUSY_INITIAL;
  stage->stalled = UNSTALLED;
  return 0;
}

int LSQ_init(LSQ_t* stage){
  // std::deque take care of itself
  stage->busy = BUSY_INITIAL;
  stage->stalled = UNSTALLED;
  return 0;
}

int CFID_init(APEX_CPU* cpu){
  for(int i=0; i<NUM_CFID; i++){
    if(i==0)
      cpu->cfid_arr[i].valid = INVALID; // because 0 is used at the begining
    else
      cpu->cfid_arr[i].valid = VALID;

    cpu->cfid_arr[i].z_flag = VALID;
    cpu->cfid_arr[i].z_flag_valid = VALID;
    cpu->cfid_arr[i].z_urf_index = UNUSED_REG_INDEX;
    memset(cpu->cfid_arr[i].rat_bak, 0xFF, sizeof(cpu->cfid_arr[i].rat_bak));
    memset(cpu->cfid_arr[i].urf_valid_bak, 0xFF, sizeof(cpu->cfid_arr[i].urf_valid_bak));
  }
  cpu->cfio.push_back(0);
  return 0;
}

int BC_init(APEX_CPU* ){
  // cpu->broadcast.entry.set(); // std::set take care of itself when you use new instead of malloc
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

/* Broadcast functions */
bool operator<(const Broadcast_base& lhl, const Broadcast_base& lhr){
  return lhl.tag < lhr.tag;
}

/*
 * fetch data for rs1 and rs2 from intFU, mulFU, mem, ROB
 * work for DRD or IQ stage
 * and set readyforIssue flags when it got all src value
 */
int fetchValue(APEX_CPU* cpu, CPU_Stage_base* entry){
  int failed1 = INVALID;
  int failed2 = INVALID;
  /* fetch rs1_value if its needed and haven't been fetched */
  if(entry->rs1 != UNUSED_REG_INDEX && entry->rs1_value_valid==INVALID){
    /* lastest instance could in ROB or
     * commited value to urf and r_rat[rs1] point to it*/
    if(entry->rs1_tag == cpu->r_rat[entry->rs1]){
      entry->rs1_value = cpu->urf[entry->rs1_tag];
      entry->rs1_value_valid = VALID;
    }
    /* lastest instance has got result */
    else if(cpu->broadcast.entry.find(Broadcast_base(entry->rs1_tag, 0x0)) != cpu->broadcast.entry.end()){
      entry->rs1_value = cpu->broadcast.entry.find(Broadcast_base{entry->rs1_tag, 0x0})->data;
      entry->rs1_value_valid = VALID;
    }
    /* lastest instance could in IQ or didn't get result, then don't copy data */
    else{
      failed1 = VALID;
    }
  }
  /* the same for rs2 */
  if(entry->rs2 != UNUSED_REG_INDEX && entry->rs2_value_valid==INVALID){
    /* lastest instance could in ROB or commited value */
    if(entry->rs2_tag == cpu->r_rat[entry->rs2]){
      entry->rs2_value = cpu->urf[entry->rs2_tag];
      entry->rs2_value_valid = VALID;
    }
    /* lastest instance has got result */
    else if(cpu->broadcast.entry.find(Broadcast_base(entry->rs2_tag, 0x0)) != cpu->broadcast.entry.end()){
      entry->rs2_value = cpu->broadcast.entry.find(Broadcast_base{entry->rs2_tag, 0x0})->data;
      entry->rs2_value_valid = VALID;
    }
    /* lastest instance could in IQ, then don't copy data */
    else{
      failed2 = VALID;
    }
  }
  /* set readyforIssue */
  /* BZ and BNZ readyforIssue bits is dependent on last arithmetic instrn */
  if(strcmp(entry->opcode, "BZ")==0 &&
      strcmp(entry->opcode, "BNZ")==0
      ){
    if(cpu->cfid_arr[entry->cfid].z_flag_valid == VALID){
      entry->readyforIssue = VALID;
    }
  }
  /* STORE R1 R2 #3 only need R2 for readyforIssue */
  else if(strcmp(entry->opcode, "STORE")==0){
    if(failed2 == INVALID ){
      entry->readyforIssue = VALID;
    }
  }
  /* if got all rs_values or never need one, set readyforIssue */
  /* JUMP and JAL is readyforIssue when it gets resource, like other instrns */
  else if(failed1 == INVALID && failed2 == INVALID){
    entry->readyforIssue = VALID;
  }
  // these are not readyforIssue
  else{
  }
  return 0;
}

/* used for LSQ STORE entry to fetch R1 value, and
 * intFU(STORE)->LSQ is only addr and adrr_valid */
int LSQ_STORE_fetchValue(APEX_CPU* cpu, CPU_Stage_base* entry){
  assert(strcmp(entry->opcode, "STORE")==0);
  assert(entry->rs1 != UNUSED_REG_INDEX);
  // int failed1 = INVALID;
  /* fetch rs1_value if its needed and haven't been fetched */
  if(entry->rs1_value_valid==INVALID){
    /* lastest instance could in commited value to urf and r_rat[rs1] point to it*/
    if(entry->rs1_tag == cpu->r_rat[entry->rs1]){
      entry->rs1_value = cpu->urf[entry->rs1_tag];
      entry->rs1_value_valid = VALID;
    }
    /* lastest instance has got result */
    else if(cpu->broadcast.entry.find(Broadcast_base(entry->rs1_tag, 0x0)) != cpu->broadcast.entry.end()){
      entry->rs1_value = cpu->broadcast.entry.find(Broadcast_base{entry->rs1_tag, 0x0})->data;
      entry->rs1_value_valid = VALID;
    }
    /* lastest instance could in IQ or didn't get result, then don't copy data */
    else{
      // failed1 = VALID;
    }
  }
  return 0;
}

/*
int isEntryReadyforIssue(APEX_CPU*, CPU_Stage_base* entry){
  if(entry->rs1 != UNUSED_REG_INDEX){
  }
}
*/

/* helper functions of flush_restore()
 * flush IQ, LSQ, ROB entries that match the cfid
 * and reset urf_valid to VALID according to rd_tag
 */
int flush(APEX_CPU* cpu, int cfid){
  assert(cfid>=0 && cfid<NUM_CFID);
  /* IQ */
  for(int i=0; i<NUM_IQ_ENTRY; ++i){
    if(cpu->iq.entry[i].valid == INVALID && cpu->iq.entry[i].cfid == cfid){
      cpu->iq.entry[i].valid = VALID;
    }
  }
  /* LSQ */
  while(cpu->lsq.entry.size() >0 &&
      cpu->lsq.entry.back().cfid == cfid
      ){
    cpu->lsq.entry.pop_back();
  }
  /* ROB */
  while(cpu->rob.entry.size() >0 &&
      cpu->rob.entry.back().cfid == cfid
      ){
    // reset urf_valid of prefetch instrns' rd_tag
    if(cpu->rob.entry.back().rd_tag != UNUSED_REG_INDEX){
      cpu->urf_valid[cpu->rob.entry.back().rd_tag] = VALID;
    }
    cpu->rob.entry.pop_back();
  }

  return 0;
}

/* don't restore urf_valid like this
 * memcpy(cpu->urf_valid, cpu->cfid_arr[cfid].urf_valid_bak, sizeof(cpu->urf_valid));
 * because there are like two parts that change urf_valid when prefetch
 * one is prefetch instrn in DRD, it may allocate new UR and change UR_valid=INVALID
 * one is commited instrn in ROB, it may deallocate UR and change UR_valid=VALID
 * only former part of changes needs to restore
 * to remove former part of changes, need track prefetch instrn and set it back to VALID
 */
/*
int restore_urf_valid(APEX_CPU* cpu, int cfid){
  for(int i=0; i<NUM_UREGS; ++i){
    if(cpu->urf_valid[i]==INVALID &&
        cpu->cfid_arr[cfid].urf_valid_bak[i]==VALID
      ){
      cpu->urf_valid[i] = VALID;
    }
  }
  return 0;
}
*/

/* public to cpu.cpp
 * flush F, DRD, and
 * flush IQ, LSQ, ROB according to the cfid(all cfids behind the cfid)
 * retore cpu->rat and cpu->urf from cpu->cfid_arr[cfid]
 */
int flush_restore(APEX_CPU* cpu, int cfid){
  setStagetoNOP(&(cpu->fetch_stage.entry));
  setStagetoNOP(&(cpu->drd.entry));
  assert(cpu->cfio.size()>0);
  int temp=0;
  while((temp=cpu->cfio.back())!=cfid){
    flush(cpu, temp);
    cpu->cfid_arr[temp].valid = VALID;
    cpu->cfio.pop_back();
  }
  memcpy(cpu->rat, cpu->cfid_arr[cfid].rat_bak, sizeof(cpu->rat));
  // restore_urf_valid(cpu, cfid);
  return 0;
}

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

int CFID_getValidEntry(APEX_CPU* cpu){
  assert(cpu->cfio.size()>0);
  int j=cpu->cfio.back();
  for(int i=j+1; (i%NUM_CFID) != j; i++){
    if(cpu->cfid_arr[i%NUM_CFID].valid == VALID){
      return i%NUM_CFID;
    }
  }
  return FAILED;
}

CPU_Stage_base*
ROB_searchEntry(APEX_CPU* cpu, int dispatch_cycle){
  if(dispatch_cycle == INVALID) return NULL;
  assert(cpu->rob.entry.size()>0);
  CPU_Stage_base* addr=NULL;
  for(auto& i: cpu->rob.entry){
    if(i.dispatch_cycle == dispatch_cycle){
      addr = &i;
      break;
    }
  }
  return addr;
}

CPU_Stage_base*
LSQ_searchEntry(APEX_CPU* cpu, int dispatch_cycle){
  if(dispatch_cycle == INVALID) return NULL;
  assert(cpu->lsq.entry.size()>0);
  CPU_Stage_base* addr=NULL;
  for(auto& i: cpu->lsq.entry){
    if(i.dispatch_cycle == dispatch_cycle){
      addr = &i;
      break;
    }
  }
  return addr;
}

int setZFlaginCFID_arr(APEX_CPU* cpu, CPU_Stage_base* entry){
  assert(entry->buffer_valid==VALID);
  for(int i=0; i<NUM_CFID; ++i){
    if(cpu->cfid_arr[i].valid == INVALID &&
        cpu->cfid_arr[i].z_urf_index == entry->rd_tag
      ){
      if(entry->buffer == 0)
        cpu->cfid_arr[i].z_flag = VALID;
      else
        cpu->cfid_arr[i].z_flag = INVALID;

      cpu->cfid_arr[i].z_flag_valid = VALID;
    }
  }
  return 0;
}

/*
 * usually isForwarded receive return value
 * SUCCEED(0) means success
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
    nextStage->busy = BUSY_NEW;
    return SUCCEED;
  }
  /* dispatch logic, DRD->IQ, DRD->ROB, DRD->LSQ */
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
        return SUCCEED;
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
        return SUCCEED;
      }
      else{
        return FAILED;
      }
    }
  }
  /* IQ->intFU or IQ->mulFU */
  /* select */
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
      nextStage->busy = BUSY_NEW;
      return SUCCEED;
    }
    /* others go to intFU */
    else{
      IntFU_t* nextStage = &cpu->intFU;
      if(nextStage->busy > BUSY_DONE || nextStage->stalled == STALLED){
        return FAILED;
      }
      nextStage->entry= *entry;
      nextStage->busy = BUSY_NEW;
      return SUCCEED;
    }
  }
  /* intFU->broadcast, intFU->ROB or intFU->LSQ */
  else if(stage_num == intFU){
    IntFU_t* stage = &cpu->intFU;
    /* LOAD or STORE, intFU->LSQ */
    if(strcmp(stage->entry.opcode, "LOAD")==0 ||
        strcmp(stage->entry.opcode, "STORE") ==0
      ){
      CPU_Stage_base* LSQ_entry_ptr = LSQ_searchEntry(cpu, stage->entry.dispatch_cycle);
      if(LSQ_entry_ptr!=NULL){
        assert(stage->entry.mem_address_valid == VALID);
        // *LSQ_entry_ptr = stage->entry;
        // don't wanna overwrite LSQ_entry's R1 value and valid bit
        // copy IQ changes
        LSQ_entry_ptr->valid = stage->entry.valid;
        LSQ_entry_ptr->readyforIssue = stage->entry.readyforIssue;
        LSQ_entry_ptr->rs2_value = stage->entry.rs2_value;
        LSQ_entry_ptr->rs2_value_valid = stage->entry.rs2_value_valid;
        // copy intFU changes
        LSQ_entry_ptr->mem_address = stage->entry.mem_address;
        LSQ_entry_ptr->mem_address_valid = stage->entry.mem_address_valid;
      }
      else {
        fprintf(stderr, "can't find the Entry in LSQ\n");
        assert(0);
      }
      return SUCCEED;
    }
    /* other instrn-> ROB and broadcast */
    else{
      /* intFU->ROB, set ROB entry buffer and valid bit */
      CPU_Stage_base* ROB_entry_ptr = ROB_searchEntry(cpu, stage->entry.dispatch_cycle);
      if(ROB_entry_ptr != NULL){
        stage->entry.completed = VALID;
        *ROB_entry_ptr = stage->entry;
      }
      else{
        fprintf(stderr, "can't find Entry in ROB\n");
        assert(0);
      }
      /* intFU->broadcast */
      if(stage->entry.rd != UNUSED_REG_INDEX){
        assert(stage->entry.rd_tag != UNUSED_REG_INDEX);
        assert(stage->entry.buffer_valid == VALID);
        cpu->broadcast.entry.insert(Broadcast_base(stage->entry.rd_tag, stage->entry.buffer));
      }
      return SUCCEED;
    }
  }
  /* mulFU->broadcast, mulFU->ROB */
  else if(stage_num == mulFU){
    /* set ROB entry buffer and valid bit */
    MulFU_t* stage = &cpu->mulFU;
    CPU_Stage_base* ROB_entry_ptr = ROB_searchEntry(cpu, stage->entry.dispatch_cycle);
    if(ROB_entry_ptr != NULL){
      stage->entry.completed = VALID;
      *ROB_entry_ptr = stage->entry;
    }
    else {
      fprintf(stderr, "can't find the Entry in LSQ\n");
      assert(0);
    }
    /* copy to cpu->broadcast */
    if(stage->entry.rd != UNUSED_REG_INDEX){
      assert(stage->entry.rd_tag != UNUSED_REG_INDEX);
      assert(stage->entry.buffer_valid == VALID);
      cpu->broadcast.entry.insert(Broadcast_base(stage->entry.rd_tag, stage->entry.buffer));
    }
    return SUCCEED;
  }
  /* LSQ->MEM */
  else if(stage_num == LSQ){
    LSQ_t* stage = &cpu->lsq;
    MEM_t* nextStage = &cpu->mem;
    assert(stage->entry.size()>0);
    CPU_Stage_base& it = stage->entry.front();
    assert(strcmp(it.opcode, "LOAD")==0 || strcmp(it.opcode, "STORE")==0);
    if(nextStage->busy > BUSY_DONE || nextStage->stalled == STALLED){
      return FAILED;
    }
    nextStage->entry= it;
    nextStage->busy = BUSY_NEW;
    return SUCCEED;
  }
  /* MEM->ROB, MEM->broadcast */
  else if(stage_num == MEM ){
    MEM_t* stage = &cpu->mem;
    /* MEM->ROB, set ROB entry buffer and valid bit */
    CPU_Stage_base* ROB_entry_ptr = ROB_searchEntry(cpu, stage->entry.dispatch_cycle);
    if(ROB_entry_ptr != NULL){
      stage->entry.completed = VALID;
      *ROB_entry_ptr = stage->entry;
    }
    else{
      fprintf(stderr, "can't find Entry in ROB\n");
      assert(0);
    }
    /* MEM->broadcast */
    if(stage->entry.rd != UNUSED_REG_INDEX){
      assert(stage->entry.rd_tag != UNUSED_REG_INDEX);
      assert(stage->entry.buffer_valid == VALID);
      cpu->broadcast.entry.insert(Broadcast_base(stage->entry.rd_tag, stage->entry.buffer));
    }
    return SUCCEED;
  }
  /* ROB or others */
  else {  // if(stage_num == ROB){
    fprintf(stderr, "should reach here\n");
    assert(0);
  }
  return FAILED;
}
