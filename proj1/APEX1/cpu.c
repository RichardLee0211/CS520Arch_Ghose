/*
 *  cpu.c
 *  Contains APEX cpu pipeline implementation
 *
 *  Author :
 *  Gaurav Kothari (gkothar1@binghamton.edu)
 *  State University of New York, Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

int enable_interactive;


/* don't bother with these functions, lower level */
  /* Converts the PC(4000 series) into
   * array index for code memory
   *
   * Note : You are not supposed to edit this function
   *
   */
  int
    get_code_index(int pc)
    {
      return (pc - PC_START_INDEX) / BYTES_PER_INS;
    }

  static void
    print_instruction(CPU_Stage* stage)
    {
      if (strcmp(stage->opcode, "STORE") == 0 ||
          strcmp(stage->opcode, "LOAD") == 0) {
        printf(
            "%s,R%d,R%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
      }

      else if (strcmp(stage->opcode, "MOVC") == 0) {
        printf("%s,R%d,#%d ", stage->opcode, stage->rd, stage->imm);
      }

      else if (strcmp(stage->opcode, "ADD") == 0 ||
          strcmp(stage->opcode, "SUB") == 0 ||
          strcmp(stage->opcode, "AND") == 0 ||
          strcmp(stage->opcode, "OR") == 0 ||
          strcmp(stage->opcode, "EX-OR") == 0 ||
          strcmp(stage->opcode, "MUL") == 0
          ){
        printf("%s,R%d,R%d,R%d", stage->opcode, stage->rd, stage->rs1, stage->rs2);
      }

      else if(strcmp(stage->opcode, "NOP") == 0){
        printf("%s", stage->opcode);
      }

      else{
        printf("%s", "UNKNOWN");
      }

    }

  /* Debug function which dumps the cpu stage
   * content
   *
   * Note : You are not supposed to edit this function
   *
   */
  static void
    print_stage_content(char* name, CPU_Stage* stage)
    {
      printf("%-15s: pc(%d) ", name, stage->pc);
      print_instruction(stage);
      printf("\n");
    }


  /* wenchen */
  void print_all_stage(CPU_Stage* stages){
    print_stage_content("Writeback", &stages[WB]);
    print_stage_content("Memory", &stages[MEM]);
    print_stage_content("Execute", &stages[EX]);
    print_stage_content("Decode/RF", &stages[DRF]);
    print_stage_content("Fetch", &stages[F]);
  }

  void print_regs(int* regs, int* regs_valid){
    printf("REGS: \n");
    for(int i=1; i<NUM_REGS; ++i){
      printf("%3d(%d)  ", regs[i], regs_valid[i]);
      if((i%(NUM_REGS/4))==0) printf("\n");
    }
    printf("\n");
  }

  void print_data_memory(int* data_memory){
    // printf("DATA_MEM: \n");
    for(int i=0; i<100; ++i){
      if(i%8==0) printf("\nDATA_MEM[%d]", i);
      printf("%3d  ", data_memory[i]);
    }
    printf("\n");
  }

  void print_flag_reg(int* flag_reg){
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
  memset(cpu->regs, 0, sizeof(int) * NUM_REGS);
  // memset(cpu->regs_valid, VALID, sizeof(int) * NUM_REGS); // remember memset set byte by byte
  // TODO_3: initial it to VALID, hope clients don't read garbadge from it
  for(int i=0; i<NUM_REGS; ++i) cpu->regs_valid[i] = VALID;
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0, sizeof(int) * DATA_MEM_SIZE);
  cpu->ins_completed=0;

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);

  if (!cpu->code_memory) {
    free(cpu);
    return NULL;
  }

  /* Make all stages busy except Fetch stage, initally to start the pipeline */
  /* wenchen
  for (int i = DRF; i < NUM_STAGES; ++i) {
    cpu->stage[i].busy = BUSY;
  }
  */

  if (ENABLE_DEBUG_MESSAGES) {
    // fprintf(stderr, // TODO: why stderr??
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
  CPU_Stage* stage = &cpu->stage[F];
  CPU_Stage* next_stage = &cpu->stage[DRF];

  if (!stage->busy && !stage->stalled) {
    /* Store current PC in fetch latch */
    stage->pc = cpu->pc;

    /*
     * initial fetch latch
     */
    if(get_code_index(cpu->pc) >= cpu->code_memory_size){
      strcpy(stage->opcode, "NOP");
      stage->rd = UNUSED_REG_INDEX;
      stage->rs1 = UNUSED_REG_INDEX;
      stage->rs2 = UNUSED_REG_INDEX;
      stage->imm = UNUSED_IMM;
    }
    else{
      APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(cpu->pc)];
      strcpy(stage->opcode, current_ins->opcode);
      stage->rd = current_ins->rd;
      stage->rs1 = current_ins->rs1;
      stage->rs2 = current_ins->rs2;
      stage->imm = current_ins->imm;
    }

    /* Update PC for next instruction */
    cpu->pc += BYTES_PER_INS;

    /* Copy data from fetch latch to decode latch*/
    if(!next_stage->busy && !next_stage->stalled)
      *next_stage = *stage;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Fetch", stage);
    }
  }
  return 0;
}

/*
 *  Decode Stage of APEX Pipeline
 */
int
decode(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[DRF];
  CPU_Stage* next_stage = &cpu->stage[EX];

  if(stage->pc == 0) return -1;

  /* if not valid?: stalled this stage */
  if( !cpu->regs_valid[stage->rs1] ||
      !cpu->regs_valid[stage->rs2] ||
      !cpu->regs_valid[stage->rd]
    ){
    stage->stalled = STALLED;
  }
  else{
    stage->stalled = UNSTALLED;
  }

  if (!stage->busy && !stage->stalled) {

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

    else if (strcmp(stage->opcode, "MOVC") == 0) {
    }

    else if(strcmp(stage->opcode, "NOP")==0){
      // nothing to do
    }

    /* Copy data from decode latch to execute latch*/
    if(!next_stage->busy && !next_stage->stalled)
      *next_stage = *stage;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Decode/RF", stage);
    }
  }
  return 0;
}

/*
 *  Execute Stage of APEX Pipeline
 */
int
execute(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[EX];
  CPU_Stage* next_stage = &cpu->stage[MEM];

  if(stage->pc == 0) return -1;

  // TODO: stalled condition ??
  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "STORE") == 0 ||
        strcmp(stage->opcode, "LOAD") == 0
        ) {
      stage->mem_address = stage->rs2_value + stage->imm; // TODO: assume it doesn't excess the boundary
    }

    else if (strcmp(stage->opcode, "ADD") == 0) {
      stage->buffer = stage->rs1_value + stage->rs2_value;
    }
    else if (strcmp(stage->opcode, "SUB") == 0) {
      stage->buffer = stage->rs1_value - stage->rs2_value;
    }
    else if (strcmp(stage->opcode, "MUL") == 0) {
      stage->buffer = stage->rs1_value * stage->rs2_value;
      // delay 2 circle
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

    else if (strcmp(stage->opcode, "MOVC") == 0) {
      // seems nothing to do with MOVC at EX stage
    }

    else if(strcmp(stage->opcode, "NOP")==0){
      // nothing to do
    }

    else{
      // unknown instruction
    }

    /* Copy data from Execute latch to Memory latch*/
    if(!next_stage->busy && !next_stage->stalled)
      *next_stage = *stage;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute", stage);
    }
  }
  return 0;
}

/*
 *  Memory Stage of APEX Pipeline
 */
int
memory(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MEM];
  CPU_Stage* next_stage = &cpu->stage[WB];

  if(stage->pc == 0) return -1;

  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "STORE") == 0) {
      cpu->data_memory[stage->mem_address] = stage->rs1_value;
    }
    else if (strcmp(stage->opcode, "LOAD") == 0) {
      stage->buffer = cpu->data_memory[stage->mem_address] ;
    }

    else {
      // MOV, +, -, *, /, &, |, ~, ^, NOP, UNKNOWN
      // seems nothing to do
    }

    /* Copy data from decode latch to execute latch*/
    if(!next_stage->busy && !next_stage->stalled)
      *next_stage = *stage;


    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Memory", stage);
    }
  }
  return 0;
}

/*
 *  Writeback Stage of APEX Pipeline
 */
int
writeback(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[WB];

  if(stage->pc == 0) return -1;

  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "STORE") == 0 ) {
      // nothing to do with STORE at WB stage
    }
    else if(strcmp(stage->opcode, "LOAD") == 0){
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = VALID;
    }

    else if (strcmp(stage->opcode, "MOVC") == 0) {
      cpu->regs[stage->rd] = stage->imm;
    }

    else if(strcmp(stage->opcode, "ADD")==0 ||
        strcmp(stage->opcode, "SUB")==0 ||
        strcmp(stage->opcode, "MUL")==0 ||
        strcmp(stage->opcode, "AND")==0 ||
        strcmp(stage->opcode, "OR")==0 ||
        strcmp(stage->opcode, "EX-OR")==0
        ){
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = VALID;
    }

    else {
      // NOP, UNKNOWN, nothing to do
    }

    cpu->ins_completed++;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Writeback", stage);
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

  char command[56];
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
    print_flag_reg(cpu->regs);
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
    if (cpu->ins_completed == cpu->code_memory_size) {
      printf("(apex) >> Simulation Complete");
      break;
    }

    if (ENABLE_DEBUG_MESSAGES) {
      printf("--------------------------------\n");
      printf("Clock Cycle #: %d\n", cpu->clock);
      printf("--------------------------------\n");
    }

    writeback(cpu);
    memory(cpu);
    execute(cpu);
    decode(cpu);
    fetch(cpu);
    cpu->clock++;

    if(enable_interactive){
      if(takeCommand(cpu)==-1){
        break;
      }
    }

  }

  return 0;
}
