
#ifndef INCLUDEDsegm
#define INCLUDEDsegm

	// supports dynamic segment lengths at SA positions

	// we assume n is a power of 2 (not allocating the unneeded bits) and
	// deploy a perfect binary tree where every node says whether the 
	// maximum in its subree is left or right. In the beginning all the 
	// values are n, so we set all left (zeros). Later one can change any
	// value

#include "bitvector.h"

typedef struct s_segm {
    uint64_t size; // number of elements in the bitmaps 
    uint64_t nlevels; // number of levels in the tree
    uint64_t first; // value where the last level starts
    uint64_t *dirs; // directions bitmaps, 0=left, 1=right in the perfect tree
    uintData *data; // a pointer to the dynamic array of numbers (shared)
    } *Tsegm;

	// creates a segment from wm and data assuming all data values are max
        // data and map are arrays to retrieve data

Tsegm segmCreate (uintData *data, uint64_t n);

	// destroys S
void segmDestroy (Tsegm S);

	// gives space of segm in w-bit words
uint64_t segmSpace (Tsegm S);

	// returns maximum pos in data for node dirs[i]
extern uint64_t segmValue (Tsegm S, uint64_t i);

        // returns maximum pos in data for a segment i..jup to val
        // it can work less if it knows that being >= val suffices
        // log^2 n as it identifies log n segm nodes where the max is
extern uint64_t cappedMax(Tsegm S, uint64_t i, uint64_t j, uintData val);

	// reflects that data[i] has been modified to val
void segmUpdate (Tsegm S, uint64_t i, uintData val);

#endif
