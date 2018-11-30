/* try to save some cpu_helper functions here */
#include"cpu_helper.h"
#include"cpu_base.h"
#include"global.h"

#include<string.h>
#include<stdio.h>

int
copyArray(int* arr1, int* arr2, int size){
  for(int i=0; i<size; ++i){
    arr1[i] = arr2[i];
  }
  return 0;
}

/*
 * Converts the PC(4000 series) into index for code memory
 */
int
get_code_index(int pc)
{
  return (pc - PC_START_INDEX) / BYTES_PER_INS;
}

int
get_pc(int code_index)
{
  return (code_index * BYTES_PER_INS + PC_START_INDEX) ;
}

/* have some basic function here */
int setStagetoNOPE(CPU_Stage* stage){
  stage->pc=0;
  strcpy(stage->opcode, "NOP");
  stage->rs1 = UNUSED_REG_INDEX;
  stage->rs2 = UNUSED_REG_INDEX;
  stage->rd = UNUSED_REG_INDEX;
  stage->imm = UNUSED_IMM;
  return 0;
}

/*
 * TODO: need rewrite this part,
 */
int copyStagetoNext(APEX_CPU* cpu, int stage_num){
  // TODO:
  return cpu->clock+stage_num;
}
