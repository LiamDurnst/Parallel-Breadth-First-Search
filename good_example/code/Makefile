#INC=-I/home/beta/cilk/include/cilk++
OPT=-O3
DBG=
#OPT=
#DBG=-g

all: search

search: _bfstest.o _bag.o
	/home/beta/cilk/bin/cilk++ -o search _bfstest.o _bag.o -lcilkutil

_bfstest.o: bfstest.cpp _bag.o
	/home/beta/cilk/bin/cilk++ -x cilk++ -c $(DBG) $(OPT) -o _bfstest.o bfstest.cpp $(INC)

_bag.o: bag.cpp bag.h
	/home/beta/cilk/bin/cilk++ -c $(DBG) $(OPT) -o _bag.o bag.cpp $(INC)

clean: 
	rm -f search _bfstest.o _bag.o
