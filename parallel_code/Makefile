CC=icc

all: pbfs-sorted pbfs-unsorted

pbfs-sorted: pbfs.cpp bag.cpp pennant.cpp
	$(CC) -o pbfs pbfs-sorted.cpp bag.cpp pennant.cpp

pbfs-unsorted: pbfs.cpp bag.cpp pennant.cpp
	$(CC) -o pbfs pbfs-unsorted.cpp bag.cpp pennant.cpp

clean:
	rm  pbfs-sorted pbfs-unsorted
