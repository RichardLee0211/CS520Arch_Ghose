/*
 *  file_parser.c
 *  Contains functions to parse input file and create
 *  code memory, you can edit this file to add new instructions
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

/*
 * This function is related to parsing input file
 */
static int
get_num_from_string(char* buffer)
{
  char str[16];
  int j = 0;
  for (int i = 0; buffer[i] != '\0'; ++i) {
    if(buffer[i] == '-' || (buffer[i]>='0' && buffer[i]<='9')){
      str[j] = buffer[i];
      j++;
    }
  }
  str[j] = '\0';
  return atoi(str);
}

/*
 * This function is related to parsing input file
 */
static void
create_APEX_instruction(APEX_Instruction* ins, char* buffer)
{
  char* token = strtok(buffer, " ,\n");
  int token_num = 0;
  char tokens[6][128];
  while (token != NULL) {
    strcpy(tokens[token_num], token);
    token_num++;
    token = strtok(NULL, " ,\n");
  }

  strcpy(ins->opcode, tokens[0]);

  /* MOVC RD #NUM */
  if (strcmp(ins->opcode, "MOVC") == 0) {
    ins->rd = get_num_from_string(tokens[1]);
    ins->rs1 = UNUSED_REG_INDEX;
    ins->rs2 = UNUSED_REG_INDEX;
    ins->imm = get_num_from_string(tokens[2]);
  }

  /* STORE R1 R2 #NUM */
  else if (strcmp(ins->opcode, "STORE") == 0 ){
    ins->rd = UNUSED_REG_INDEX;
    ins->rs1 = get_num_from_string(tokens[1]);
    ins->rs2 = get_num_from_string(tokens[2]);
    ins->imm = get_num_from_string(tokens[3]);
  }
  /* LOAD RD R2 #NUM3 */
  else if(strcmp(ins->opcode, "LOAD") == 0) {
    ins->rd = get_num_from_string(tokens[1]);
    ins->rs1 = UNUSED_REG_INDEX;
    ins->rs2 = get_num_from_string(tokens[2]);
    ins->imm = get_num_from_string(tokens[3]);
  }

  /* ADD RD R2 R3 */
  else if (strcmp(ins->opcode, "ADD") == 0 ||
      strcmp(ins->opcode, "SUB") == 0 ||
      strcmp(ins->opcode, "AND") == 0 ||
      strcmp(ins->opcode, "OR") == 0 ||
      strcmp(ins->opcode, "EX-OR") == 0
      ) {
    ins->rd = get_num_from_string(tokens[1]);
    ins->rs1 = get_num_from_string(tokens[2]);
    ins->rs2 = get_num_from_string(tokens[3]);
    ins->imm = UNUSED_IMM;
  }

  else if(strcmp(ins->opcode, "MUL") == 0 ){
    ins->rd = get_num_from_string(tokens[1]);
    ins->rs1 = get_num_from_string(tokens[2]);
    ins->rs2 = get_num_from_string(tokens[3]);
    ins->imm = UNUSED_IMM;
  }

  /* JMP R1, #2 #absolute addr */
  else if(strcmp(ins->opcode, "JUMP")==0 ||
      strcmp(ins->opcode, "JMP")==0
      ){
    ins->rd = UNUSED_REG_INDEX;
    ins->rs1 = get_num_from_string(tokens[1]);
    ins->rs2 = UNUSED_REG_INDEX;
    ins->imm = get_num_from_string(tokens[2]);
  }
  /* BZ #4, relative addr */
  else if(strcmp(ins->opcode, "BZ")==0 ||
      strcmp(ins->opcode, "BNZ")==0
      ){
    ins->rd = UNUSED_REG_INDEX;
    ins->rs1 = UNUSED_REG_INDEX;
    ins->rs2 = UNUSED_REG_INDEX;
    ins->imm = get_num_from_string(tokens[1]);
  }
  /* HALT */
  else if(strcmp(ins->opcode, "HALT")==0){
    ins->rd = UNUSED_REG_INDEX;
    ins->rs1 = UNUSED_REG_INDEX;
    ins->rs2 = UNUSED_REG_INDEX;
    ins->imm = UNUSED_IMM;
  }
  /* PRINT_REG R1 */
  else if(strcmp(ins->opcode, "PRINT_REG")==0){
    ins->rd = UNUSED_REG_INDEX;
    ins->rs1 = get_num_from_string(tokens[1]);
    ins->rs2 = UNUSED_REG_INDEX;
    ins->imm = UNUSED_IMM;
  }
  /* PRINT_MEM R1 #2 */
  else if(strcmp(ins->opcode, "PRINT_MEM")==0){
    ins->rd = UNUSED_REG_INDEX;
    ins->rs1 = get_num_from_string(tokens[1]);
    ins->rs2 = UNUSED_REG_INDEX;
    ins->imm = get_num_from_string(tokens[2]);
  }

  /* UNKNOWN instruction */
  else{
    strcpy(ins->opcode, "UNKNOWN");
    ins->rd = UNUSED_REG_INDEX;
    ins->rs1 = UNUSED_REG_INDEX;
    ins->rs2 = UNUSED_REG_INDEX;
    ins->imm = UNUSED_IMM;
  }

}

/*
 * This function is related to parsing input file
 */
APEX_Instruction*
create_code_memory(const char* filename, int* size)
{
  if (!filename) {
    return NULL;
  }

  FILE* fp = fopen(filename, "r");
  if (!fp) {
    return NULL;
  }

  char* line = NULL;
  size_t len = 0;
  ssize_t nread;
  int code_memory_size = 0;

  while ((nread = getline(&line, &len, fp)) != -1) {
    if(line[0] != '#' && line[0]!='\n') // for # comment
      code_memory_size++;
  }
  *size = code_memory_size;
  if (!code_memory_size) {
    fclose(fp);
    return NULL;
  }

  APEX_Instruction* code_memory =
    malloc(sizeof(*code_memory) * code_memory_size);
  if (!code_memory) {
    fclose(fp);
    return NULL;
  }

  rewind(fp);
  int current_instruction = 0;
  while ((nread = getline(&line, &len, fp)) != -1) {
    if(line[0] != '#' && line[0]!= '\n'){ // for # comments
      create_APEX_instruction(&code_memory[current_instruction], line);
      current_instruction++;
    }
  }

  free(line);
  fclose(fp);
  return code_memory;
}
