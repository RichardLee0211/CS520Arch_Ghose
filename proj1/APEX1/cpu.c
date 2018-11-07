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
      if (strcmp(stage->opcode, "STORE") == 0) {
        printf(
            "%s,R%d,R%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
      }

      if (strcmp(stage->opcode, "MOVC") == 0) {
        printf("%s,R%d,#%d ", stage->opcode, stage->rd, stage->imm);
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
  cpu->pc = PC_START_INDEX;
  memset(cpu->regs, 0, sizeof(int) * NUM_REGS);
  memset(cpu->regs_valid, UNVALID, sizeof(int) * NUM_REGS);
  cpu->regs_valid[0] = VALID; // wenchen: regs[0] is always valid, works as zero register
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0, sizeof(int) * 4000); // TODO: wenchen: data_memory is 4096 ints

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);

  if (!cpu->code_memory) {
    free(cpu);
    return NULL;
  }

  /* Make all stages busy except Fetch stage, initally to start the pipeline */
  for (int i = DRF; i < NUM_STAGES; ++i) {
    cpu->stage[i].busy = BUSY;
  }

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

    /* Index into code memory using this pc and copy all instruction fields into
     * fetch latch
     */
    APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(cpu->pc)];
    strcpy(stage->opcode, current_ins->opcode);
    stage->rd = current_ins->rd;
    stage->rs1 = current_ins->rs1;
    stage->rs2 = current_ins->rs2;
    stage->imm = current_ins->imm;
    stage->rd = current_ins->rd;

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

  /* what if not valid?: stalled this stage */
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

    /* Read data from register file for store */
    if (strcmp(stage->opcode, "STORE") == 0) {
        stage->rs1_value = cpu->regs[stage->rs1];
        stage->rs2_value = cpu->regs[stage->rs2];
    }

    /* No Register file read needed for MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {
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
  // TODO: stalled condition ??
  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "STORE") == 0) {
      stage->mem_address = stage->rs2_value + stage->imm; // TODO: assume it doesn't excess the boundary
    }

    if (strcmp(stage->opcode, "MOVC") == 0) {
      // seems nothing to do with MOVC at EX stage
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
  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "STORE") == 0) {
      cpu->data_memory[stage->mem_address] = stage->rs1_value;
    }

    if (strcmp(stage->opcode, "MOVC") == 0) {
      // seems nothing to do with MOVC at MEM stage
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
  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "STORE") == 0) {
      // nothing to do with STORE at WB stage
    }

    if (strcmp(stage->opcode, "MOVC") == 0) {
      /* Update register file */
      // cpu->regs[stage->rd] = stage->buffer; // TODO: why buffer
      cpu->regs[stage->rd] = stage->imm;
    }

    cpu->ins_completed++;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Writeback", stage);
    }
  }
  return 0;
}

/*
 *  APEX CPU simulation loop
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
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
  }

  return 0;
}
