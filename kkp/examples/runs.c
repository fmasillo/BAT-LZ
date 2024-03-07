
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void main(int argc, char **argv)

   { FILE *fileT = fopen(argv[1],"r");
     FILE *fileA = fopen(argv[2],"r");
     int n,s,r,i;
     char *text;
     int *sa;
     struct stat st;
     stat(argv[1], &st);
     n = st.st_size;
     text = malloc(n+1);
     sa = malloc(n*sizeof(int));
     text[0]=0; text++;
     fread (text,1,n,fileT);
     fread (sa,sizeof(int),n,fileA);
     r = s = 1;
     for (i=1;i<n;i++)
	 { if (text[sa[i]] != text[sa[i-1]]) s++; 
	   if (text[sa[i]-1] != text[sa[i-1]-1]) r++; }
     printf ("%i chars\n%i runs\n",s,r);
   }
