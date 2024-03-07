#include "suffix_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************************************************************************************************/
/**************************************************************************************************/

/*Prints all possible command-line arguments and then exits the program.

  Input : None:

  Output: A printout of all the command-line options to the screen.*/

/*The program entry point. Handles all command line arguments. 
This function calls suffix tree interface functions in order to 
create, search and delete a suffix tree out of a string or a 
file given as input. 

  Input : Number of command-line parameters, an array of command-line parameters.
  
  Output: Returns 0 to the operating system.*/


/**************************************************************************************************/
int main(int argc, char* argv[])
{
	SUFFIX_TREE* tree;
	unsigned char command, *str = NULL, *filename, freestr = 0;
	FILE* file = 0;
	DBL_WORD i,z,len = 0;

	if(argc < 3) {
	   fprintf(stderr,"Usage: %s <filename> <maxc>\n",argv[0]); 
	   exit(1);
	}
		filename = argv[1];
		file = fopen(filename,"r");
		/*Check for validity of the file.*/
		if(file == 0)
		{
			printf("can't open file.\n");
			return(0);
		}
		/*Calculate the file length in bytes. This will be the length of the source string.*/
		fseek(file, 0, SEEK_END);
		len = ftell(file);
		fseek(file, 0, SEEK_SET);
		str = (unsigned char*)malloc((len+1)*sizeof(unsigned char));
		if(str == 0)
		{
			printf("\nOut of memory.\n");
			exit(0);
		}
		fread(str, sizeof(unsigned char), len, file);
{ unsigned i;
  for (i=0;i<len;i++)
    if (str[i] == 0)
       { fprintf(stderr,"Cannot process this string, it has zeros inside\n");
 	 exit(1);
       }
}
  		str[len] = 0;

	fprintf(stderr,"Constructing tree...\n");
	tree = ST_CreateTree(str,len); // it appends the 0 anyway
	fprintf(stderr,"Parsing...\n");
        tree->COST = atoi(argv[2]);
	char *filename_cost = (char*)malloc(strlen(filename)+45);
	strcpy(filename_cost,filename);
	char *strCost = (char *)malloc(sizeof(char)*10);
	sprintf(strCost, "%d", tree->COST);
	strcat(filename_cost,"_greedier");
	strcat(filename_cost,strCost);
	strcat(filename_cost,".cost");
	fprintf(stderr, "filename_cost: %s\n",filename_cost);
	z = parseBLZ(tree);
	fprintf(stderr,"%i phrases\n",z);
	
	// store in filename.cost the costArray of the tree
	{ 
	  FILE *f;
	  f = fopen(filename_cost,"w");
	  if (f == 0)
	     { fprintf(stderr,"Cannot open %s\n",filename_cost);
	       exit(1);
	     }
	  int i;
	  for(i=0; i < len; i++)
	  	fprintf(f,"%i\n",tree->costArray[i]);
	  fclose(f);
	}
	return 0;
}
