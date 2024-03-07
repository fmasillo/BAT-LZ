

	// supports static wavelet matrices 

#include "wmatrix.h"

	// creates a wavelet matrix from data[0..n-1] using lev lowest bits
	// bitrank parameter k

wmatrix wmCreate (uint64_t n, uint16_t lev, uintData *data, uint k)

   { uint l;
     uint64_t i,z,o;
     uintData *data1,*data2,*datas;
     data1 = data;
     data2 = myalloc(n*sizeof(uintData));
     wmatrix M = myalloc(sizeof(struct s_wmatrix));
     M->size = n;
     M->nlevels = lev;
     M->levels = myalloc(lev*sizeof(bitvector));
     M->zeros = myalloc(lev*sizeof(uint64_t));
     for (l=0;l<lev;l++)
	{ M->levels[l] = bitsCreate(n);
	  o = 0; // count the 1s
	  for (i=0;i<n;i++)
	      { if (data1[i] & (((uintData)1)<<(lev-1-l))) o++; }
	  z = 0; o = n-o; // where to start writing the 0s and 1s
	  for (i=0;i<n;i++)
	      { uint v = data1[i] & (((uintData)1)<<(lev-1-l));
		bitsWrite(M->levels[l],i,v);
		if (v) data2[o++] = data1[i]; else data2[z++] = data1[i];
	      }
	  bitsRankPreprocess (M->levels[l],k);
	  M->zeros[l] = z;
	  datas = data1; data1 = data2; data2 = datas;
	}
     if (data == data2) 
	{ for (i=0; i<n; i++) data[i] = data1[i];
	  myfree(data1);
	}
     else myfree(data2); 
     return M;
   }

	// destroys M
void wmDestroy (wmatrix M)

   { uint l;
     for (l=0;l<M->nlevels;l++) bitsDestroy(M->levels[l]);
     myfree(M->levels);
     myfree(M->zeros);
     myfree(M);
   }

	// gives space of wmatrix in w-bit words

uint64_t wmSpace (wmatrix M)

   { return M->nlevels*bitsSpace(M->levels[0]) + M->nlevels + 
	    sizeof(struct s_wmatrix)/(w/8);
   }

	// tracks down i from level l to level l+1

extern inline uint64_t wmTrackDown (wmatrix M, uint16_t l, uint64_t i)

   { uint64_t r = bitsRank(M->levels[l],i);
     if (bitsAccess(M->levels[l],i)) return M->zeros[l] - 1 + r;
     return i - r;
   }

	// tracks down [*i,*j] from level l to level l+1, left or right
extern inline void wmTrackLeftRange (wmatrix M, uint16_t l, 
				     int64_t *i, int64_t *j)

   { if (*i) *i -= bitsRank(M->levels[l],*i-1);
     *j -= bitsRank(M->levels[l],*j);
   }

extern inline void wmTrackRightRange (wmatrix M, uint16_t l, 
				      int64_t *i, int64_t *j)

   { *i = M->zeros[l] + (*i ? bitsRank(M->levels[l],*i-1) : 0);
     *j = M->zeros[l] - 1 + bitsRank(M->levels[l],*j);
   }

