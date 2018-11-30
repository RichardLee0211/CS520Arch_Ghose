#ifndef _FILE_PARSER_H_
#define _FILE_PARSER_H_

typedef struct APEX_Instruction APEX_Instruction; // forward declare, data type from cpu.h

/* public to cpu.c */
APEX_Instruction*
create_code_memory(const char* filename, int* size);

#endif /* _FILE_PARSER_H_ */
