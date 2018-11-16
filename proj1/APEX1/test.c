
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
  // try to do stupid instruction decode

  char *buffer;
  size_t len;
  // scanf("%s", buffer);
  getline(&buffer, &len, stdin);
  char* token = strtok(buffer, " ,");
  int token_num = 0;
  char tokens[6][128];
  while (token != NULL) {
    strcpy(tokens[token_num], token);
    token_num++;
    token = strtok(NULL, " ,");
  }
  for(int i=0; i<6; i++){
    printf("%c\n", tokens[i][0]);
  }
  /*
  uint32_t flags = 0;
  flags |= 0x1;
  printf("%d\n", flags);
  flags &= ~0x1;
  printf("%d\n", flags);
  */
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
