
	// supports dynamic segment lengths at SA positions

	// we assume n is a power of 2 (not allocating the unneeded bits) and
	// deploy a perfect binary tree where every node says whether the 
	// maximum in its subree is left or right. In the beginning all the 
	// values are n, so we set all left (zeros). Later one can change any
	// value

#include "segm_greedier.h"

	// creates a segment from data[0..n-1] assuming all values are n
	// data and map are arrays to retrieve data

Tsegm segmCreate (uintData *data, uint64_t n)

   { uint64_t i,j,s;
     Tsegm S = myalloc(sizeof(struct s_segm));
     S->size = n;
     S->nlevels = numbits(n);
     S->first = (((uint64_t)1) << numbits(n)) - 1;
     s = (2*n+w-2)/w;
     S->dirs = myalloc(s*sizeof(uint64_t));
     for (i=0;i<s;i++) S->dirs[i] = 0; // all left, assume all data = n
     S->data = data;
     return S;
   }

	// destroys S

void segmDestroy (Tsegm S)

   { myfree(S->dirs);
     myfree(S);
   }

	// gives space of segm in w-bit words

uint64_t segmSpace (Tsegm S)

   { return ((2*S->size+w-2)/w) + sizeof(struct s_segm)/(w/8);
   }

#define ancestor(i,l) ((((i)+1)>>(l))-1)
#define parent(i) ancestor(i,1)
#define left(i) (2*(i)+1)
#define right(i) (2*(i)+2)

	// returns maximum pos in data for node dirs[i]
	// log n time

extern uint64_t segmValue (Tsegm S, uint64_t i)

   { while (i < S->first)
       { i = 2*i+1+bitsAccessA(S->dirs,i); }
     i -= S->first; // where the max value is
     if (i >= S->size) return S->size; // address out of bounds
     return i;
   }

        // returns maximum pos in data for a segment i..jup to val
        // it can work less if it knows that being >= val suffices
        // log^2 n as it identifies log n segm nodes where the max is

static uint64_t cappedmax (Tsegm S, uint64_t i, uint64_t j, uintData val,
		           uint64_t node, uint nodel)

   { uint64_t span = ((uint64_t)1) << (S->nlevels-nodel);
     uint64_t from = ((node+1) << (S->nlevels-nodel)) - 1 - S->first;
     uint64_t to = from + span - 1;
     uintData v1,v2;
     uint64_t pos1,pos2;
     while (span > 1)
	{ if (i >= from + span/2) 
	     { node = right(node); from += span/2; }
          else if (j < from + span/2)
	     { node = left(node); to -= span/2; }
	  else break;
	  nodel++; span /= 2;
	}
	// check if we got the exact node
     if ((i == from) && (j == to)) return segmValue(S,node);
	// otherwise, the search divides in two
     pos1 = cappedmax (S,i,from+span/2-1,val,left(node),nodel+1);
     v1 = S->data[pos1];
     if (v1 >= val) return pos1;
     pos2 = cappedmax (S,from+span/2,j,val,right(node),nodel+1);
     v2 = S->data[pos2];
     if (v1 >= v2) return pos1; else return pos2;
   }

extern uint64_t cappedMax (Tsegm S, uint64_t i, uint64_t j, uintData val)

   { if (i >= j) return i;
     return cappedmax(S,i,j,val,0,0);
   }

	// reflects that data[i] has been modified 
	// log^2 n time:
		// log n segm levels 
		// log n time per segm level because segmValues are not explicit

void segmUpdate (Tsegm S, uint64_t i, uintData v)

   { uint64_t p,pa,a,sa; // nodes a, parent of a, sibling of a...
     a = i + S->first; // the leaf corresponding to data[i];
     while (a)
	{ uint d;
	  uintData sv;
  	  if (v == S->size) break; // maximum value, cannot change things
	  pa = parent(a);
	  d = bitsAccessA(S->dirs,pa);
	  if (a != 2*pa+1+d) break; // does not point anymore towards i
	  sa = 2*pa+1+(1-d); // the sibling of a
	  p = segmValue(S,sa); // pos of sibling (might be out of bounds)
	  if (p < S->size) // if it exists
	     { sv = S->data[p]; // value of sibling
	       if (sv > v) // pa should cease pointing to a
	          { bitsWriteA(S->dirs,pa,1-d);
	            v = sv; // for the ancestors
	          }
	     }
	  a = pa;
	}
   }


