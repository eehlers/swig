
LIB = ../lib/libSimpleLib.so

all: $(LIB)

$(LIB): adder.o
	g++ -shared -fPIC -g -Wall -Wno-unused-local-typedefs -o $(LIB) adder.o

adder.o: adder.cpp adder.hpp
	g++ -c -fPIC -g -Wall -Wno-unused-local-typedefs -o adder.o -I.. adder.cpp

clean:
	rm -f $(LIB) *.o

