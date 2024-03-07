
	// supports dynamic segment lengths at SA positions

	// we assume n is a power of 2 (not allocating the unneeded bits) and
	// deploy a perfect binary tree where every node says whether the 
	// maximum in its subree is left or right. In the beginning all the 
	// values are n, so we set all left (zeros). Later one can change any
	// value

#include "segm.h"

	// creates a segment from data[0..n-1] assuming all values are n
	// data and map are arrays to retrieve data

segm segmCreate (wmatrix wm, uintData *data, uintData *map)

   { uint64_t i,j,s;
     uint64_t n = wm->size;
     uint l = wm->nlevels;
     segm S = myalloc(sizeof(struct s_segm));
     S->wm = wm;
     S->size = n;
     S->nlevels = l;
     S->first = (((uint64_t)1) << numbits(n)) - 1;
     s = (2*n+w-2)/w;
     S->dirs = myalloc(l * sizeof(uint64_t*));
     for (j=0;j<l;j++)
        { S->dirs[j] = myalloc(s*sizeof(uint64_t));
          for (i=0;i<s;i++) S->dirs[j][i] = 0; // all left, assume all data = n
	}
     S->data = data;
     S->map = map;
     return S;
   }

	// destroys S

void segmDestroy (segm S)

   { uint i;
     for (i=0;i<S->nlevels;i++) myfree(S->dirs[i]);
     myfree(S->dirs);
     myfree(S);
   }

	// gives space of segm in w-bit words

uint64_t segmSpace (segm S)

   { return S->nlevels*((2*S->size+w-2)/w) + S->nlevels + 
	    sizeof(struct s_segm)/(w/8);
   }

#define ancestor(i,l) ((((i)+1)>>(l))-1)
#define parent(i) ancestor(i,1)
#define left(i) (2*(i)+1)
#define right(i) (2*(i)+2)

	// returns maximum pos in data for node dirs[i] level l
	// log n time, first in segment tree and then track in wm

extern uint64_t segmValue (segm S, uint64_t i, uint l)

   { while (i < S->first)
       { i = 2*i+1+bitsAccessA(S->dirs[l],i); }
     i -= S->first; // where the max value is in level l, now downwards
     if (i >= S->size) return S->size; // address out of bounds
     while (l < S->nlevels)
           i = wmTrackDown(S->wm,l++,i);
     return i;
   }

	// returns pos in data for a segment i..j, level l, up to val
	// it can work less if it knows that being >= val suffices
	// log^2 n as it identifies log n segm nodes where the max is

static uint64_t cappedmax (segm S, uint l, uint64_t i, uint64_t j, uintData val,
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
     if ((i == from) && (j == to)) return segmValue(S,node,l);
	// otherwise, the search divides in two
     pos1 = cappedmax (S,l,i,from+span/2-1,val,left(node),nodel+1);
     v1 = S->data[S->map[pos1]];
     if (v1 >= val) return pos1;
     pos2 = cappedmax (S,l,from+span/2,j,val,right(node),nodel+1);
     v2 = S->data[S->map[pos2]];
     if (v1 >= v2) return pos1; else return pos2;
   }

extern uint64_t cappedMax (segm S, uint l, uint64_t i, uint64_t j, uintData val)

   { return cappedmax(S,l,i,j,val,0,0);
   }

	// reflects that data[i] has been modified 
	// where i is the position at the wm root		
	// log^3 n time:
		// log n wt levels, log n segm levels in each wt level,
		// log n time per segm level because segmValues are not explicit

static void update (segm S, uint64_t i, uint l, uintData v)

   { uint64_t p,pa,a,sa; // nodes a, parent of a, sibling of a...
     a = i + S->first; // the leaf corresponding to data[i];
     while (a)
	{ uint d;
	  uintData sv;
  	  if (v == S->size) break; // maximum value, cannot change things
	  pa = parent(a);
	  d = bitsAccessA(S->dirs[l],pa);
	  if (a != 2*pa+1+d) break; // does not point anymore towards i
	  sa = 2*pa+1+(1-d); // the sibling of a
	  p = segmValue(S,sa,l); // pos of sibling (might be out of bounds)
	  if (p < S->size) // if it exists
	     { sv = S->data[S->map[p]]; // value of sibling
	       if (sv > v) // pa should cease pointing to a
	          { bitsWriteA(S->dirs[l],pa,1-d);
	            v = sv; // for the ancestors
	          }
	     }
	  a = pa;
	}
   }

void segmUpdate (segm S, uint64_t i, uintData val)

   { uint l;
     for (l=0;l<S->nlevels;l++)
	 { update (S,i,l,val);
	   i = wmTrackDown(S->wm,l,i);
	 }
   }

