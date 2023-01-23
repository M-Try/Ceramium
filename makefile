CXX := g++
BIN := libceramium.1.o
CXXFLAGS := 
OUTFLAGS := -o $(BIN)

all: main

main:
	$(CXX) $(CXXFLAGS) $(OUTFLAGS) src/*.cpp

debug:
	$(CXX) $(CXXFLAGS) -g $(OUTFLAGS) src/*.cpp

test:
	$(CXX) $(BIN) test/testcera.h -o test/testcera.ELF

clean:
	rm $(BIN)
	rm test/testcera.ELF