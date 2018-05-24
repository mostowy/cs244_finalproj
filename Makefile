CXXFLAGS = -std=c++14 -Wall -Werror -O0 -g
CXX = g++

OBJECTS = RunTests.o Hashes.o ChainedHashTable.o SecondChoiceHashTable.o LinearProbingHashTable.o RobinHoodHashTable.o CuckooHashTable.o

default: run-tests

run-tests: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

RunTests.o: RunTests.cpp Timing.h Hashes.h ChainedHashTable.h SecondChoiceHashTable.h LinearProbingHashTable.h RobinHoodHashTable.h CuckooHashTable.h Timer.h

%.o: %.cpp %.h Hashes.h

clean:
	rm -f run-tests *.o *~
