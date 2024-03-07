COMPILER = gcc
DFLAGS = -Wall -O9 -g 
CFLAGS = -c
OFLAGS = -o
EXECNAME = gensa baseline1_BATLZ baseline2_BATLZ greedy_BATLZ greedier_BATLZ minmax_BATLZ

all: uncompress baseline1_BATLZ baseline2_BATLZ greedy_BATLZ greedier_BATLZ minmax_BATLZ

uncompress: uncompress.o
	make -C kkp/examples/
	mv kkp/examples/gensa ./gensa
	gcc -o uncompress ${DFLAGS} uncompress.c

# from folder kkp/examples/ make there, then copy gensa to the root folder

baseline1_BATLZ: baseline1_BATLZ.o wmatrix.o basics.o bitvector.o segm.o
	${COMPILER} ${DFLAGS} baseline1_BATLZ.o wmatrix.o basics.o bitvector.o segm.o ${OFLAGS} baseline1_BATLZ

baseline2_BATLZ: baseline2_BATLZ.o wmatrix.o basics.o bitvector.o segm.o
	${COMPILER} ${DFLAGS} baseline2_BATLZ.o wmatrix.o basics.o bitvector.o segm.o ${OFLAGS} baseline2_BATLZ

greedy_BATLZ: greedy_BATLZ.o wmatrix.o basics.o bitvector.o segm.o
	${COMPILER} ${DFLAGS} greedy_BATLZ.o wmatrix.o basics.o bitvector.o segm.o ${OFLAGS} greedy_BATLZ

greedier_BATLZ: greedier_BATLZ.o basics.o bitvector.o segm_greedier.o
	${COMPILER} ${DFLAGS} greedier_BATLZ.o basics.o bitvector.o segm_greedier.o ${OFLAGS} greedier_BATLZ

minmax_BATLZ: minmax_BATLZ.o basics.o bitvector.o segm_greedier.o	
	${COMPILER} ${DFLAGS} minmax_BATLZ.o basics.o bitvector.o segm_greedier.o ${OFLAGS} minmax_BATLZ

baseline1_BATLZ.o: baseline1_BATLZ.c bitvector.h wmatrix.h segm.h basics.h
	${COMPILER} ${DFLAGS} -c baseline1_BATLZ.c

baseline2_BATLZ.o: baseline2_BATLZ.c bitvector.h wmatrix.h segm.h basics.h
	${COMPILER} ${DFLAGS} -c baseline2_BATLZ.c

greedy_BATLZ.o: greedy_BATLZ.c bitvector.h wmatrix.h segm.h basics.h
	${COMPILER} ${DFLAGS} -c greedy_BATLZ.c 

greedier_BATLZ.o: greedier_BATLZ.c segm_greedier.h suffix_tree.h basics.h
	${COMPILER} ${DFLAGS} -c greedier_BATLZ.c

minmax_BATLZ.o: minmax_BATLZ.c segm_greedier.h suffix_tree.h basics.h
	${COMPILER} ${DFLAGS} -c minmax_BATLZ.c

main.o:	suffix_tree.h 
	${COMPILER} ${DFLAGS} ${CFLAGS} main.c 

clean: 
	rm *.o 
	rm ${EXECNAME} uncompress
	make -C kkp/examples/ clean

basics.o: basics.c basics.h
	${COMPILER} ${DFLAGS} -c basics.c

bitvector.o: bitvector.c bitvector.h basics.h
	${COMPILER} ${DFLAGS} -c bitvector.c

segm.o: segm.c segm.h bitvector.h basics.h
	${COMPILER} ${DFLAGS} -c segm.c

segm_greedier.o: segm_greedier.c segm_greedier.h bitvector.h basics.h
	${COMPILER} ${DFLAGS} -c segm_greedier.c

wmatrix.o: wmatrix.c wmatrix.h bitvector.h basics.h
	${COMPILER} ${DFLAGS} -c wmatrix.c
