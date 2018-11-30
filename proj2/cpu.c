/*
 *  cpu.c
 *  Contains APEX cpu pipeline implementation
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define NDEBUG
#include <assert.h>

#include "cpu.h"
#include "auxiliary.h"
#include "file_parser.h"

/* globle between files */
int enable_interactive;

/* wenchen */
int
copyArray(int* arr1, int* arr2, int size){
  for(int i=0; i<size; ++i){
    arr1[i] = arr2[i];
  }
  return 0;
}

/* wenchen */
int setStagetoNOPE(CPU_Stage* stage){
  stage->pc=0;
  strcpy(stage->opcode, "NOP");
  stage->rs1 = UNUSED_REG_INDEX;
  stage->rs2 = UNUSED_REG_INDEX;
  stage->rd = UNUSED_REG_INDEX;
  stage->imm = UNUSED_IMM;
  return 0;
}

/* wenchen
 * TODO: need rewrite this part,
 */
int copyStagetoNext(APEX_CPU* cpu, int stage_num){
}


/*
 * This function creates and initializes APEX cpu.
 */
APEX_CPU*
APEX_cpu_init(const char* filename)
{
  if (!filename) {
    return NULL;
  }

  APEX_CPU* cpu = malloc(sizeof(*cpu));
  if (!cpu) {
    return NULL;
  }

  /* Initialize PC, Registers and all pipeline stages */
  cpu->clock = 0;
  cpu->pc = PC_START_INDEX;
  cpu->flags = 0x0;
  memset(cpu->regs, 0, sizeof(int) * NUM_REGS);
  // memset(cpu->regs_valid, VALID, sizeof(int) * NUM_REGS); // remember memset set byte by byte
  // TODO_3: initial it to VALID, hope clients don't read garbadge from it
  for(int i=0; i<NUM_REGS; ++i) cpu->regs_valid[i] = VALID;
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0xFF, sizeof(int) * DATA_MEM_SIZE); // set to -1 for debug purpose
  cpu->ins_completed=0;

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);

  if (!cpu->code_memory) {
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
 *  Fetch Stage of APEX Pipeline
 */
int
fetch(APEX_CPU* cpu)
{
  int stage_num = F;
  CPU_Stage* stage = &cpu->stage[stage_num];

  stage->stalled = UNSTALLED;

  /* Copy data from fetch latch to decode latch or fail */
  // TODO_3: may it should ret -1 when failed and set stalled bit here
  copyStagetoNext(cpu, stage_num);

  /* if fetch is busy or stalled, don't fetch from instr_mem */
  /*
  if(stage->delay[stage_num]>0 || stage->stalled==STALLED){
    return 0;
  }
  */

  stage->pc = cpu->pc; /* Store current PC in fetch latch */
  cpu->pc += BYTES_PER_INS; /* Update PC for next instruction */

  /* fetch instr from instr_mem to latch */
  if(stage->pc >= get_pc(cpu->code_memory_size)){
    setStagetoNOPE(stage);
    stage->pc = get_pc(cpu->code_memory_size); // because setStagetoNOPE set stage->pc to zero
  }
  else{
    APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(stage->pc)];
    strcpy(stage->opcode, current_ins->opcode);
    stage->rd = current_ins->rd;
    stage->rs1 = current_ins->rs1;
    stage->rs2 = current_ins->rs2;
    stage->imm = current_ins->imm;
    // copyArray(stage->delay, current_ins->delay, NUM_STAGES);
  }
  return 0;
}

/*
 *  Decode/Renaming/Dispatch Stage of APEX Pipeline
 */
int
decode(APEX_CPU* cpu)
{
  int stage_num = DRF;
  CPU_Stage* stage = &cpu->stage[stage_num];

  stage->stalled = UNSTALLED; // initial to be UNSTALLED, until don't get resource or fail copying to next_stage

  /* regs is valid or already belong to me, e.g. ADD R1 R1 R2 */
  if( (cpu->regs_valid[stage->rs1]== VALID || cpu->regs_valid[stage->rs1]== stage->pc) &&
      (cpu->regs_valid[stage->rs2]== VALID || cpu->regs_valid[stage->rs2]== stage->pc)
      // && (cpu->regs_valid[stage->rd]== VALID || cpu->regs_valid[stage->rd]== stage->pc)
    ){
    /* MOVC, +, -, *, /, &, |, ~, ^, STORE, LOAD */
    if (strcmp(stage->opcode, "STORE")==0 ||
        strcmp(stage->opcode, "LOAD")==0 ||
        strcmp(stage->opcode, "ADD")==0 ||
        strcmp(stage->opcode, "SUB")==0 ||
        strcmp(stage->opcode, "AND")==0 ||
        strcmp(stage->opcode, "OR")==0 ||
        strcmp(stage->opcode, "EX-OR")==0 ||
        strcmp(stage->opcode, "MUL")==0
        ) {
      stage->rs1_value = cpu->regs[stage->rs1];
      stage->rs2_value = cpu->regs[stage->rs2];
    }

    else if (strcmp(stage->opcode, "JUMP") == 0 ||
        strcmp(stage->opcode, "JMP") == 0
        ) {
      stage->rs1_value = cpu->regs[stage->rs1];
    }

    else if (strcmp(stage->opcode, "HALT") == 0
        ) {
      cpu->pc = get_pc(cpu->code_memory_size);
      setStagetoNOPE(&cpu->stage[F]);
      cpu->stage[F].pc= get_pc(cpu->code_memory_size);
    }

    else if (strcmp(stage->opcode, "PRINT_REG") == 0 ||
        strcmp(stage->opcode, "PRINT_MEM")==0
        ) {
      stage->rs1_value= cpu->regs[stage->rs1];
    }

    else {
      // MOVC, BZ, BNZ, NOP, UNKNOWN nothing to do
    }

    stage->delay[stage_num]--;
    stage->stalled = UNSTALLED;
  }
#if FORWARD_ENABLE
  /* data forwarding case
   * at least one of rs2, rs3 is in EX or EX+1 or MEM+1 stage
   */
#endif

  /* Copy data from decode latch to execute latch*/
  int ret = copyStagetoNext(cpu, stage_num);

  /* set regs_valid to this pc if copy success */
  // if(stage->stalled == UNSTALLED && stage->delay[stage_num] <= 0){
  if(ret == 0 && stage->rd != UNUSED_REG_INDEX){
      cpu->regs_valid[stage->rd] = stage->pc;
  }
  return 0;
}

/*
 *  Execute Stage of APEX Pipeline
 */
int
execute(APEX_CPU* cpu)
{
  int stage_num = EX;
  CPU_Stage* stage = &cpu->stage[stage_num];


  /*
  if (stage->delay[stage_num]>0) {
    if (strcmp(stage->opcode, "STORE") == 0 ||
        strcmp(stage->opcode, "LOAD") == 0
        ) {
      stage->mem_address = stage->rs2_value + stage->imm; // TODO_3: assume it doesn't excess the boundary
    }

    else if(strcmp(stage->opcode, "MOVC")==0){
      stage->buffer = stage->imm;
    }

    else if (strcmp(stage->opcode, "ADD") == 0) {
      stage->buffer = stage->rs1_value + stage->rs2_value;
      if(stage->buffer == 0) cpu->flags |= 0x1;
      else cpu->flags &= ~0x1;
    }
    else if (strcmp(stage->opcode, "SUB") == 0) {
      stage->buffer = stage->rs1_value - stage->rs2_value;
      if(stage->buffer == 0) cpu->flags |= 0x1;
      else cpu->flags &= ~0x1;
    }
    else if (strcmp(stage->opcode, "MUL") == 0) {
      stage->buffer = stage->rs1_value * stage->rs2_value;
      if(stage->buffer == 0) cpu->flags |= 0x1;
      else cpu->flags &= ~0x1;
    }
    else if (strcmp(stage->opcode, "AND") == 0) {
      stage->buffer = stage->rs1_value & stage->rs2_value;
    }
    else if (strcmp(stage->opcode, "OR") == 0) {
      stage->buffer = stage->rs1_value | stage->rs2_value;
    }
    else if (strcmp(stage->opcode, "EX-OR") == 0) {
      stage->buffer = stage->rs1_value ^ stage->rs2_value;
    }

    else if(strcmp(stage->opcode, "JUMP")==0 ||
        strcmp(stage->opcode, "JMP") ==0
        ){
      stage->buffer= stage->rs1_value + stage->imm;
      cpu->pc = stage->pc + stage->buffer;
      if(cpu->pc < PC_START_INDEX ) cpu->pc = PC_START_INDEX;
      if(cpu->pc > get_pc(cpu->code_memory_size)) cpu->pc = get_pc(cpu->code_memory_size);
      setStagetoNOPE(&cpu->stage[F]);
      setStagetoNOPE(&cpu->stage[DRF]);
    }

    else if(strcmp(stage->opcode, "BZ")==0){
      if((cpu->flags&0x1) == 0x1){
        cpu->pc = stage->pc + stage->imm;
        if(cpu->pc < PC_START_INDEX ) cpu->pc = PC_START_INDEX;
        if(cpu->pc > get_pc(cpu->code_memory_size)) cpu->pc = get_pc(cpu->code_memory_size);
        setStagetoNOPE(&cpu->stage[F]);
        setStagetoNOPE(&cpu->stage[DRF]);
      }
    }
    else if(strcmp(stage->opcode, "BNZ")==0){
      if((cpu->flags&0x1) == 0x0){
        cpu->pc = stage->pc + stage->imm;
        if(cpu->pc < PC_START_INDEX ) cpu->pc = PC_START_INDEX;
        if(cpu->pc > get_pc(cpu->code_memory_size)) cpu->pc = get_pc(cpu->code_memory_size);
        setStagetoNOPE(&cpu->stage[F]);
        setStagetoNOPE(&cpu->stage[DRF]);
      }
    }

    else if(strcmp(stage->opcode, "PRINT_MEM")==0){
      stage->mem_address = stage->rs1_value + stage->imm;
    }

    else{
      // MOVC, PIRNT_REG, NOP, unknown instruction
    }

    stage->delay[stage_num] --;
    stage->stalled = UNSTALLED;
  }
  */

  /* Copy data from Execute latch to Memory latch*/
  copyStagetoNext(cpu, stage_num);

  return 0;
}

/*
 *  Memory Stage of APEX Pipeline
 */
int
memory(APEX_CPU* cpu)
{
  int stage_num = MEM;
  CPU_Stage* stage = &cpu->stage[stage_num];

  /*
  if (stage->delay[stage_num]>0) {

    if (strcmp(stage->opcode, "STORE") == 0) {
      cpu->data_memory[stage->mem_address] = stage->rs1_value;
    }
    else if (strcmp(stage->opcode, "LOAD") == 0) {
      stage->buffer = cpu->data_memory[stage->mem_address] ;
    }

    else if(strcmp(stage->opcode, "PRINT_MEM")==0){
      printf("MEM[%d]: %d\n", stage->mem_address, cpu->data_memory[stage->mem_address]);
    }

    else {
      // MOV, +, -, *, /, &, |, ~, ^, NOP, UNKNOWN
      // seems nothing to do
    }

    stage->delay[stage_num]--;
    stage->stalled = UNSTALLED;
  }
  */

  /* Copy data from decode latch to execute latch*/
  copyStagetoNext(cpu, stage_num);

  return 0;
}

/*
 *  Writeback Stage of APEX Pipeline
 */
int
writeback(APEX_CPU* cpu)
{
  int stage_num = WB;
  CPU_Stage* stage = &cpu->stage[stage_num];

  /*
  if (stage->delay[stage_num]>0) {

    if (strcmp(stage->opcode, "STORE") == 0 ) {
      // nothing to do with STORE at WB stage
    }
    else if(strcmp(stage->opcode, "LOAD") == 0){
      cpu->regs[stage->rd] = stage->buffer;
      if(cpu->regs_valid[stage->rd]==stage->pc)
        cpu->regs_valid[stage->rd] = VALID;
    }

    else if (strcmp(stage->opcode, "MOVC") == 0) {
      cpu->regs[stage->rd] = stage->buffer;
      if(cpu->regs_valid[stage->rd]==stage->pc)
        cpu->regs_valid[stage->rd] = VALID;
    }

    else if(strcmp(stage->opcode, "ADD")==0 ||
        strcmp(stage->opcode, "SUB")==0 ||
        strcmp(stage->opcode, "MUL")==0
        ){
      cpu->regs[stage->rd] = stage->buffer;
      if(cpu->regs_valid[stage->rd]==stage->pc)
        cpu->regs_valid[stage->rd] = VALID;
    }

    else if(strcmp(stage->opcode, "AND")==0 ||
        strcmp(stage->opcode, "OR")==0 ||
        strcmp(stage->opcode, "EX-OR")==0
        ){
      cpu->regs[stage->rd] = stage->buffer;
      if(cpu->regs_valid[stage->rd]==stage->pc)
        cpu->regs_valid[stage->rd] = VALID;
    }


    else if(strcmp(stage->opcode, "PRINT_REG")==0){
      printf("R%d: %d\n", stage->rs1, cpu->regs[stage->rs1]);
    }

    else {
      // PRINT_MEM, NOP, UNKNOWN, nothing to do
    }
    stage->delay[stage_num]--;
    stage->stalled = UNSTALLED;
    if(strcmp(stage->opcode, "NOP")!=0)
      cpu->ins_completed++;
  }
  */

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

/*
 *  APEX CPU simulation loop
 */
int
APEX_cpu_run(APEX_CPU* cpu)
{
  while (1) {

    /* All the instructions committed, so exit */
    if (cpu->stage[WB].pc >= get_pc(cpu->code_memory_size)) {
      printf("(apex) >> Simulation Complete");
      break;
    }

    writeback(cpu);
    memory(cpu);
    execute(cpu);
    decode(cpu);
    fetch(cpu);
    cpu->clock++;

    if (ENABLE_DEBUG_MESSAGES) {
      printf("--------------------------------\n");
      printf("after Clock Cycle #: %d\n", cpu->clock);
      printf("--------------------------------\n");
      print_all_stage(cpu->stage);
    }


    if(enable_interactive){
      if(takeCommand(cpu)==-1){
        break;
      }
    }

  }

  return 0;
}
