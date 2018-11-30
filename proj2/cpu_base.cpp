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

  // printf(" %s", stage->stalled ? "STALLED":"UNSTALLED");
}

void print_Fetch_stage(Fetch_t* stage){
  printf("%-15s: pc(%04d) ", "Fetch", stage->stage.pc);
  print_instruction(&stage->stage);
  printf(" %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

void print_DRD(DRD_t* stage){
  printf("%-15s: pc(%04d) ", "DRD", stage->stage.pc);
  print_instruction(&stage->stage);
  printf(" %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

void print_IQ(IQ_t* stage){
  for(int i{0}; i<NUM_IQ_ENTRY; ++i){
    printf("%-15s[%02d]: pc(%04d) ", "IQ", i, stage->entry[i].pc);
    print_instruction(&stage->entry[i]);
  }
  printf("IQ: %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

void print_ROB(ROB_t* stage){
  for(auto i: stage->entry){
    printf("%-15s: pc(%04d) ", "ROB",  i.pc);
    // print_instruction(i);
  }
  printf("IQ: %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

void print_intFU(IntFU_t* stage){
  printf("%-15s: pc(%04d) ", "DRD", stage->stage.pc);
  print_instruction(&stage->stage);
  printf(" %s %d\n", stage->stalled ? "STALLED": "UNSTALLED", stage->busy);
}

void print_mulFU(MulFU_t* stage){
  printf("%-15s: pc(%04d) ", "DRD", stage->stage.pc);
  print_instruction(&stage->stage);
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
  print_Fetch_stage(&cpu->fetch_stage);
  // print_DRD(&cpu->drd);
  // print_IQ(&cpu->iq);
  // print_intFU(&cpu->intFU);
  // print_ROB(&cpu->rob);
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
int stage_base_init(CPU_Stage_base* stage_base){
  memset(stage_base, 0xFF, sizeof(*stage_base));
  memset(stage_base->opcode, 0x00, OPCODE_SIZE*sizeof(char));
  stage_base->imm = UNUSED_IMM;
  return 0;
}

int Fetch_stage_init(Fetch_t* stage){
  stage_base_init(&stage->latch);
  stage_base_init(&stage->stage);
  stage->busy = BUSY_DONE;
  stage->stalled = UNSTALLED;
  return 0;
}

int DRD_init(DRD_t* stage){
  stage_base_init(&stage->latch);
  stage_base_init(&stage->stage);
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
  // stage->entry; // std::queue will take care of it
  stage->busy = BUSY_DONE;
  stage->stalled = UNSTALLED;
  return 0;
}

int IntFU_init(IntFU_t* stage){
  stage_base_init(&stage->latch);
  stage_base_init(&stage->stage);
  stage->busy = BUSY_DONE;
  stage->stalled = UNSTALLED;
  return 0;
}
