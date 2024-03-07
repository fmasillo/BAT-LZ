
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main (int argc, char **argv)
{ 
  if(argc != 2) 
  { 
    printf("Usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  FILE *f = fopen(argv[1],"r");
  
  if(f == NULL) 
  { 
    printf("Cannot open %s\n", argv[1]);
    exit(1);
  }
  
  // read first line to parse n: "n = xxx"
  int n;
  fscanf(f,"n = %i\n",&n);
  fprintf(stderr, "n = %i\n",n);
  //   int n = atoi(argv[2]);
  char *text = malloc(n+2);
  int i,j;

  i = 0;
  while (i < n)
  { 
    int pos,len,car;
    fscanf(f,"(%i,%i,%i)\n",&pos,&len,&car);
    //  printf("(%i,%i,%i)\n",pos,len,car);
    if (car < 0) car += 256;
    if (len) 
    { 
      for (j=0;j<len;j++) text[i+j] = text[pos+j];
    } 
    text[i+len] = car;
    i += len+1;
  }
  text[i-1]=0;
  printf("%s",text);

  free(text);
}
