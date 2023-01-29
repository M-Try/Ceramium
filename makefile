CXX := g++
BIN := libceramium.1.o
CXXFLAGS :=
OUTFLAGS := -o $(BIN)
LIBFLAGS := -I.

all: main

main:
	$(CXX) $(CXXFLAGS) $(OUTFLAGS) src/*.cpp $(LIBFLAGS)

debug:
	$(CXX) $(CXXFLAGS) -g $(OUTFLAGS) src/*.cpp $(LIBFLAGS)

test:
	$(CXX) $(BIN) test/testcera.cpp -o test/testcera.ELF

clean:
	rm $(BIN)
	rm test/testcera.ELF