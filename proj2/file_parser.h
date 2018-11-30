#ifndef _FILE_PARSER_H_
#define _FILE_PARSER_H_

#include "cpu_base.h"

/* public to cpu.c, using cpu_base.h */
APEX_Instruction*
create_code_memory(const char* filename, int* size);

#endif /* _FILE_PARSER_H_ */
