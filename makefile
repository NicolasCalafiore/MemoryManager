CXX = g++

CXXFLAGS = -std=c++2a -Wall -I.

DEPS = MemoryManager.h

OBJ = MemoryManager.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

libMemoryManager.a: $(OBJ)
	ar rcs $@ $^
