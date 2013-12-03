
LIB = ../lib/libSimpleLib.so

all: $(LIB)

$(LIB): adder.o foo.o
	g++ -shared -fPIC -g -Wall -o $(LIB) adder.o foo.o

adder.o: adder.cpp adder.hpp
	g++ -c -fPIC -g -Wall -o adder.o -I.. adder.cpp

foo.o: foo.cpp foo.hpp
	g++ -c -fPIC -g -Wall -o foo.o -I.. foo.cpp

clean:
	rm -f $(LIB) *.o

