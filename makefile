CXX := g++
BIN := vm
CXXFLAGS := -g
OUTFLAGS := -o $(BIN)

all: main

main:
	$(CXX) $(CXXFLAGS) $(OUTFLAGS) vm.cpp

clean:
	rm $(BIN)
