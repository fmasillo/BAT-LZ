
// baseline: first LZ and then cut wherever U mod c+1 = 0

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

	// only 32-bit version wrt A, to avoid excessive space
	// change uintA to uint64_t to handle longer texts

#include "segm.h"

#define K 4  // space/time tradeoff for bitmaps

uint MAX;    // max number of copies allowed

byte *T; // null-terminated text 

uint64_t n; // text length

uintData *SA, *ISA, *Map; // SA and its inverse, created from outside
		// Map undoes the scrambling of the wm (maybe there is a 
		// better solution)

uintData *D; // distance to next unusable pos, 0 for unusable pos

uintData *U; // number of uses (length of chain)

wmatrix M; // wmatrix of SA

segm S; // segment structures, one per wm level


// initializes all the structures, given SA and ISA of length n
// and maximum allowed chain length
void initialize (uint64_t n, uint64_t maxChain)
{ 
  uint64_t i;
  uint depth;
	// create the data D and U
  D = myalloc (n*sizeof(uintData));
  for (i=0;i<n;i++) D[i] = n; // maximum bound for all positions
  U = myalloc (n*sizeof(uintData));
	// create wavelet matrix on the SA
  depth = numbits(n);

  fprintf(stderr,"Creating wavelet matrix... "); fflush(stderr);

  M = wmCreate (n,depth,SA,K);

  fprintf(stderr,"done\n");

  Map = SA; // SA was scrambled by wmCreate

  // create the segments, one per wmatrix level 

  fprintf(stderr,"Creating chain structures... "); fflush(stderr);

  S = segmCreate(M,D,Map); 

  fprintf(stderr,"done\n");

	// stores the maximum number of copies
  if (maxChain == 0)
  { 
    fprintf (stderr,"maxchain must be positive or parse is trivial\n");
	  exit(1);
  }
  MAX = maxChain;
}

#define nomax ((uint64_t)~0)

// finds the longest admissible phrase T[i..], returning matching length
// and source
static uint64_t check (segm S, int64_t i, int64_t sp, int64_t ep, 
		       uintData val)
{ 
  uint lev = 0;
  int p = numbits(S->size);
  uint64_t v,maxv;
  int64_t nsp,nep;

  maxv = nomax;
  while ((i >= 0) && (sp <= ep))
  { 
    p--;
    if (i >= (1<<p)-1) // all the left part is inside
    { 
      nsp = sp; nep = ep;
      wmTrackLeftRange (S->wm,lev,&nsp,&nep);
      if (nsp <= nep)
      { 
        v = cappedMax (S,lev+1,nsp,nep,val);
        v = Map[v];
        if (D[v] >= val) return v;
        if ((maxv == nomax) || (D[v] > D[maxv])) maxv = v;
      }
      i -= (1<<p);
      wmTrackRightRange (S->wm,lev,&sp,&ep);
    }
    else
    { 
      wmTrackLeftRange (S->wm,lev,&sp,&ep);
    }
	  lev++;
  }

  return maxv;
}

// SA[sp..ep] corresponds to T[i..i+len-1], extend by 1 char and 
// descend the whole stree edge
uint64_t restrictRange (int64_t sp, int64_t ep, uint64_t i, uint64_t len,
		   int64_t *nsp, int64_t *nep)
{ 
  int64_t p,m,om;
  byte c,c0 = T[i+len];
  while (sp <= ep)
  { 
    m = (sp+ep)/2;
    c = T[SA[m]+len];
    if (c == c0) break; 

    if (c < c0) sp = m+1; 
    else ep = m-1;
  }
  if (sp > ep) 
  {
    *nsp = sp; 
    *nep = ep; 
    return len; 
  }
  // char found at m
  p = om = m;
  *nsp = sp;
  while (*nsp < p)
  {
    m = (*nsp+p)/2;
    c = T[SA[m]+len];
    if (c == c0) p = m; 
    else *nsp = m+1;
  }
  p = om;
  *nep = ep;
  while (*nep > p)
  {
    m = (*nep+p+1)/2;
    c = T[SA[m]+len];
    if (c == c0) p = m; 
    else *nep = m-1;
  }
  if (*nsp == *nep) 
    return n-SA[*nsp]; // stree leaf
  
  while (T[SA[*nsp]+len] == T[SA[*nep]+len]) len++;
  
  return len; 
}

uint64_t nextPhrase (uint64_t i, uint64_t *source)
{ 
  uint64_t n = S->size; // text length
  int64_t sp,ep,nsp,nep;
  uint64_t len,l,maxl,lenpos;

  len = 0; sp = 0; ep = n-1;

  while (1) // text terminator ensures termination here
  { 
    l = restrictRange (sp,ep,i,len,&nsp,&nep);
    if (nsp > nep) break; // cannot be extended even to len
    maxl = check (S,i-1,nsp,nep,l);
    if ((maxl == nomax) || (D[maxl] <= len)) break; // has no points 
    *source = maxl; len = D[*source];
    sp = nsp; ep = nep; 
    if (len < l) break; // cannot be extended beyond l
    len = l;
  }

  return len;
}


// phrase T[i..j] = T[pi..] and T[j] is explicit
// update D and U. last unusable position was last (can be -1 at first)
// returns new value of last
void copyPhrase (uint64_t i, uint64_t j, uint64_t pi)
{ 
  int64_t k,s;
  s = pi;
  for (k = i; k < j; k++) 
	{ 
    U[k] = U[s]+1;
	  s++; 
    if (s == i) s = pi; // for self-overlapping phrases     
    if (k % (1024*1024) == 0) fprintf(stderr,"%li MB\n",k/1024/1024);
	}
  if (k % (1024*1024) == 0) fprintf(stderr,"%li MB\n",k/1024/1024);
  
  U[j] = 0; // explicit char
}

bool file_exists (char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

void main (int argc, char **argv)
{
  uint64_t z,i,len,source;
  int64_t last = -1;
  struct stat st;
  FILE *f;
  char fname[1024];
  char fnameSA[1024];

  if (argc < 2)
  { 
    fprintf(stderr,"Usage: %s <filename> [<maxchain>]\n"
    "There must exist <filename>.sa as well\n"
          "No maxchain uses infinity and yields the max chain\n"
    "Redirect output to save/discard tuples\n\n",argv[0]);
    exit(1);
  }

  fprintf(stderr,"Reading text and suffix array files... "); fflush(stderr);

  strcpy (fname,argv[1]);
  strcpy (fnameSA,argv[1]);
  strcat(fnameSA,".sa");
  stat(fname,&st);
  n = st.st_size;
  f = fopen(fname,"r");
  if (f == NULL)
  { 
    fprintf(stderr,"Cannot open %s\n",fname);
    exit(1);
  }
  T = myalloc(n+1);
  fread (T,1,n,f);
  fclose(f);
  T[n] = 0; 

  if (!file_exists(fnameSA)) {
    fprintf(stderr,"File %s does not exist, creating it\n",fnameSA);
    char cmdbuf[1024];
    snprintf (cmdbuf, sizeof(cmdbuf), "./gensa %s %s", fname, fnameSA);
    int ret = system(cmdbuf);
    if (ret != 0) {
      fprintf(stderr,"Error creating %s\n",fnameSA);
      exit(1);
    }
  }

  strcat(fname,".sa");
  f = fopen(fname,"r");
  if (f == NULL)
  { 
    fprintf(stderr,"Cannot open %s\n",fname);
	  exit(1);
  }
  SA = myalloc((n+1)*sizeof(uintData));
  SA[0] = n; // simulate the final \0, kkp does not add it
  fread (SA+1,sizeof(uintData),n,f);
  fclose(f);
  n++;

  ISA = myalloc(n*sizeof(uintData));
  for (i=0;i<n;i++) ISA[SA[i]] = i;

  fprintf(stderr,"done\n");

  initialize(n,argc == 2 ? n : atoi(argv[2]));

	// recover SA after wm construction, to save space
    SA = myalloc(n*sizeof(uintData));
    for (i=0;i<n;i++) SA[ISA[i]] = i; 

	// parsing

  fprintf(stderr,"Parsing starts, n = %li\n",n); 

	// 1st phrase hardcoded to avoid border cases

  fprintf(stderr,"File %s, n = %li, parsed with maxchain = %i\n\n", argv[1],n,atoi(argv[2]));
  printf("n = %d\n",n);
  printf ("(0,0,%d)\n",T[0]);
  copyPhrase (0,0,0);
  i = 1; z = 1;
  
  while (i < n)
  { 
    len = nextPhrase(i,&source);
    copyPhrase(i,i+len,source);
    if (len == 0) 
    {
      // printf("U[i] = %d\n", U[i]);
      printf("(0,0,%d)\n",T[i]);
      z++;
    }
    else
    { 
      int lenOut = 0;
      int sourceOut = source;
      int currentLen = 0;
      // printf("source = %d, len = %d\n", source, len);
      while(lenOut < len)
      {
        while(U[i+lenOut] % (MAX+1) != 0 && lenOut < len)
        {
          // printf("U[i+lenOut] = %d\n", U[i+lenOut]);
          lenOut++;
          currentLen++;
        }
        // if(U[i+lenOut] % (MAX+1) == 0)
        //   printf("U[i+lenOut] = %d\n", U[i+lenOut]);
        printf ("(%d,%d,%d)\n",sourceOut,currentLen,T[i+lenOut]);
        lenOut++;
        sourceOut = source+lenOut;
        currentLen = 0;
        z++;
      }
      if(sourceOut <= source+len)
      {
        // printf("TO WRITE\n");
        // for(int j = sourceOut; j < source+len; j++)
        // {
        //   printf("U[j] = %d\n", U[j]);
        // }
        printf ("(%d,%d,%d)\n",sourceOut,source+len-sourceOut,T[i+len]);
        z++;
      }
    }
    i += len+1;
  }

  printf("\nz = %li\n",z);
  fprintf(stderr,"\n\nz = %li\n",z);

  // // print U
  // for(int i = 0; i < n; i++)
  // {
  //   printf("U[%d] = %d\n", i, U[i]);
  // }

  // // now cut the phrases that exceed max
  // if (argc > 2)
  // { 
  //   uint64_t zp;
  //   MAX = atoi(argv[2]);
  //   while (MAX > 0)
  //   { 
  //     zp = z; 
  //     for (i = 0; i < n; i++)
  //       if (U[i] && (U[i] % (MAX+1) == 0)) 
  //         zp++;
  //     printf("\nwith c = %li: %li phrases\n",MAX,zp);
  //     fprintf(stderr,"\n\nwith c = %li: %li phrases\n",MAX,zp);
  //     MAX -= 5;
  //   }
	// }

  // if (argc == 2)
  // { 
  //   uint64_t u = 0;
  //   for (i=0;i<n;i++) if (U[i] > u) u = U[i];
  //   fprintf(stderr,"Maximum chain length = %li\n",u);
  // }
  // fprintf(stderr,"\n");

  free(T); free(SA); free(ISA); free(Map); free(D); free(U);
  exit(0);
}
