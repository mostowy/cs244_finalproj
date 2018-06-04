CXXFLAGS = -std=c++14 -Wall -Werror -O0 -g
CXX = g++

OBJECTS = RunTests.o Hashes.o CuckooFilter.o BucketsTable.o QuotientFilter.o SSPerm.o SemiSortTable.o SSCuckooFilter.o

default: run-tests

run-tests: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

RunTests.o: RunTests.cpp Timing.h Hashes.h  CuckooFilter.h Timer.h BucketsTable.h QuotientFilter.h SSPerm.h SemiSortTable.h SSCuckooFilter.h

%.o: %.cpp %.h Hashes.h

clean:
	rm -f run-tests *.o *~
