CC=icc

all: pbfs-sorted pbfs-unsorted

pbfs-sorted: pbfs-sorted.cpp bag.cpp pennant.cpp
	$(CC) -o pbfs-sorted pbfs-sorted.cpp bag.cpp pennant.cpp

pbfs-unsorted: pbfs-unsorted.cpp bag.cpp pennant.cpp
	$(CC) -o pbfs-unsorted pbfs-unsorted.cpp bag.cpp pennant.cpp

clean:
	rm  pbfs-sorted pbfs-unsorted
