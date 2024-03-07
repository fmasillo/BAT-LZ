
#ifndef INCLUDEDsegm
#define INCLUDEDsegm

	// supports dynamic segment lengths at SA positions

	// we assume n is a power of 2 (not allocating the unneeded bits) and
	// deploy a perfect binary tree where every node says whether the 
	// maximum in its subree is left or right. In the beginning all the 
	// values are n, so we set all left (zeros). Later one can change any
	// value

#include "wmatrix.h"

typedef struct s_segm {
    wmatrix wm; // base wmatrix
    uint nlevels; // number of bitmaps (same as wm)
    uint64_t size; // number of elements in the bitmaps (same as wm)
    uint64_t first; // value where the last level starts
    uint64_t **dirs; // directions bitmaps, 0=left, 1=right in the perfect tree
    uintData *data; // a pointer to the dynamic array of numbers (shared)
    uintData *map; // a pointer to a mapping array to retrieve data (shared)
    } *segm;

	// creates a segment from wm and data assuming all data values are max
        // data and map are arrays to retrieve data

segm segmCreate (wmatrix wm, uintData *data, uintData *map);

	// destroys S
void segmDestroy (segm S);

	// gives space of segm in w-bit words
uint64_t segmSpace (segm S);

	// returns maximum pos in data for node dirs[i] level l
extern uint64_t segmValue (segm S, uint64_t i, uint l);

        // returns maximum pos in data for a segment i..j, level l, up to val
        // it can work less if it knows that being >= val suffices
        // log^2 n as it identifies log n segm nodes where the max is
extern uint64_t cappedMax(segm S, uint l, uint64_t i, uint64_t j, uintData val);

	// reflects that data[i] has been modified to val
	// where i is the position at the wm root
void segmUpdate (segm S, uint64_t i, uintData val);

#endif
