
#ifndef INCLUDEDwmatrix
#define INCLUDEDwmatrix

	// supports static wavelet matrices 

#include "bitvector.h"

typedef struct s_wmatrix {
    uint64_t size; // number of bits in each level
    uint16_t nlevels; // number of levels
    bitvector* levels; // the nlevels bitvectors
    uint64_t *zeros; // #of 0s en the bitvectors, for navigation
    } *wmatrix;

typedef uint32_t uintData; // convert to 64 bits to handle longer texts

	// creates a wavelet matrix from data[0..n-1] using lev lowest bits
	// bitrank parameter k

wmatrix wmCreate (uint64_t n, uint16_t lev, uintData *data, uint k);

	// destroys M
void wmDestroy (wmatrix M);

	// gives space of wmatrix in w-bit words
uint64_t wmSpace (wmatrix M);

	// tracks down i from level l to level l+1
extern inline uint64_t wmTrackDown (wmatrix M, uint16_t l, uint64_t i);

	// tracks down [*i,*j] from level l to level l+1, left or right
extern inline void wmTrackLeftRange (wmatrix M, uint16_t l, 
				     int64_t *i, int64_t *j);
extern inline void wmTrackRightRange (wmatrix M, uint16_t l, 
				      int64_t *i, int64_t *j);

#endif
