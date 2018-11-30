/*
 *  main.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

int
main(int argc, char const* argv[])
{
  if (argc < 2) {
    fprintf(stderr, "APEX_Help : Usage %s <input_file> <-i>\n", argv[0]);
    exit(1);
  }

  if(argc == 3 && strcmp(argv[2], "-i")==0){
    enable_interactive=1;
  }

  APEX_CPU* cpu = APEX_cpu_init(argv[1]);
  if (!cpu) {
    fprintf(stderr, "APEX_Error : Unable to initialize CPU\n");
    exit(1);
  }

  APEX_cpu_run(cpu);
  APEX_cpu_stop(cpu);
  return 0;
}
