
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>

int translate(){
  FILE* sim = fopen("apex_sim", "r");
  if(!sim){
    return -1;
  }
  char ctemp=0;
  char* line = NULL;
  size_t len = 0;
  ssize_t nread;

  bool first=true;
  while(!feof(sim)){
    fscanf(sim, "%c", &ctemp);
    if(ctemp == 0x00) {
      if(first){
        printf("\n");
        first=false;
      }
      break;
    }
    printf("0x%X", ctemp);
    first=true;
  }
}

int main(){
  uint32_t flags = 0;
  flags |= 0x1;
  printf("%d\n", flags);
  flags &= ~0x1;
  printf("%d\n", flags);
  // translate();
  /* test of getline */
  /*
  char *line=NULL;
  size_t len=0;
  int nread=0;
  while((nread=getline(&line, &len, stdin)) != -1){
    printf("%s, len: %lu\n", line, len);
  }
  */
}
