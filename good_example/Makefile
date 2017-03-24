# Makefile for bfstest.cpp
# by Dane Pitkin and Nathan Crandall

# Compiler 
CC=icc
# Debug
DEBUG=-g -O3
# Compiler flags 
CFLAGS=-Wall $(DEBUG) -I /home/beta/cilk/include/cilk++ -c
# Linking flags
LDFLAGS=-fcilkscreen
# Source files
SOURCES=pbfs.cpp Bag.cpp Pennant.cpp
# Object files
OBJECTS=$(SOURCES:.cpp=.o)
# Executable
EXECUTABLES=pbfs
# Arguments
STARTINGVERTEX=0

# Textfile for input
INPUT=input/million_edges_rmat.txt

# qsub
SMALL= qsub -I -q small -l walltime=00:30:00
LARGE= qsub -I -q large -l walltime=00:30:00




.PHONY: clean run small large runs rune runp runw runq runl

all: $(SOURCES) $(EXECUTABLES)

$(EXECUTABLES): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	g++ -o graph_viewer -lGL -lGLU -lglut graph_viewer.cpp

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

clean:
	-rm -f *.o
	-rm -f $(EXECUTABLES)
	rm -f graph_viewer

run:	runs rune runp runw runq runl

runl:
	./$(EXECUTABLES) $(STARTINGVERTEX) -cilk_set_worker_count 32 < $(INPUT)
runq:
	./$(EXECUTABLES) $(STARTINGVERTEX) -cilk_set_worker_count 16 < $(INPUT)
runw:
	./$(EXECUTABLES) $(STARTINGVERTEX) -cilk_set_worker_count 8 < $(INPUT)  
runp:
	./$(EXECUTABLES) $(STARTINGVERTEX) -cilk_set_worker_count 4 < $(INPUT)
rune:
	./$(EXECUTABLES) $(STARTINGVERTEX) -cilk_set_worker_count 2 < $(INPUT) 
runs:
	./$(EXECUTABLES) $(STARTINGVERTEX) -cilk_set_worker_count 1 < $(INPUT)
graph:
	g++ -o graph_viewer -lGL -lGLU -lglut graph_viewer.cpp


small:
	$(SMALL)

large:
	$(LARGE)