SHELL = /bin/bash
CC = g++
CPPFLAGS = -O3 -DNDEBUG -funroll-loops -static -Wall -Wextra -Wshadow -pedantic

all: count gensa

count:
	$(CC) $(CPPFLAGS) -o count count.cpp common.cpp ../algorithm/kkp.cpp
gensa:
	$(CC) $(CPPFLAGS) -o gensa gensa.cpp common.cpp divsufsort.c

clean:
	/bin/rm -f count gensa *.o
