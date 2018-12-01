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
int mem_run(APEX_CPU* cpu);
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
  ROB_init(&cpu->rob);
  IntFU_init(&cpu->intFU);

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

    Fetch_run(cpu);
    DRD_run(cpu);
    // IQ_run(cpu);
    intFU_run(cpu);
    // mulFU_run(cpu);
    // LSQ_run(cpu);
    // mem_run(cpu);
    ROB_run(cpu);
    cpu->clock++;

    if (ENABLE_DEBUG_MESSAGES) {
      printf("--------------------------------\n");
      printf("after Clock Cycle #: %d\n", cpu->clock);
      printf("--------------------------------\n");
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
  stage->busy--;

  int forwardfailed = 1;
  /* it's done, try to copy data to nextStage */
  if(stage->busy==BUSY_DONE){
    forwardfailed = copyStagetoNext(cpu, stage_num);
  }

  /* forward failed */
  if(forwardfailed == 1){
    stage->stalled = STALLED;
  }
  /* forward success, fetch next work from code_memory */
  else{
    stage->busy = BUSY_NEW;
    stage->stage.pc = cpu->pc;
    cpu->pc += BYTES_PER_INS; // Update cpu->pc so it always point to unfetch instrn

    if(stage->stage.pc >= get_pc(cpu->code_memory_size)){
      setStagetoNOP(&stage->stage);
      stage->stage.pc = get_pc(cpu->code_memory_size); // because setStagetoNOPE set stage->pc to zero
    }
    else {
      APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(stage->stage.pc)];
      strcpy(stage->stage.opcode, current_ins->opcode);
      stage->stage.rd = current_ins->rd;
      stage->stage.rs1 = current_ins->rs1;
      stage->stage.rs2 = current_ins->rs2;
      stage->stage.imm = current_ins->imm;
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

  /* if it's new data, start busy_clock from begining */
  if(stage->busy == BUSY_NEW){
    stage->busy = BUSY_DEFAULT;
  }

  /* do the job */
  // TODO: fetch, renameing
  stage->busy--;

  int forwardfailed = 1;
  /* it's done, try to copy data to nextStage */
  if(stage->busy==BUSY_DONE){
    forwardfailed = copyStagetoNext(cpu, stage_num);
  }

  /* forward failed */
  if(forwardfailed == 1){
    stage->stalled = STALLED;
  }
  /* forward success, fetch next work from code_memory */
  else{
    // TODO: is lastStage must copy data to stage->latch ??
    stage->stage = stage->latch;
    stage->busy = BUSY_NEW;
  }
  return 0;
}

/*
 *  Execute Stage of APEX Pipeline
 */
int
intFU_run(APEX_CPU* cpu)
{
  cpu++;
  return 0;
}

int
mulFU_run(APEX_CPU* cpu){
  // TODO:
  cpu++;
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
mem_run(APEX_CPU* cpu)
{
  // TODO:
  cpu++;
  return 0;
}

int
ROB_run(APEX_CPU* cpu){
  cpu++;
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
