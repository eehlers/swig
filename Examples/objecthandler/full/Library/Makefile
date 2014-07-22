
LIB = ../lib/libSimpleLib.so

all: $(LIB)

$(LIB): adder.o adder2.o foo.o
	g++ -shared -fPIC -g -Wall -o $(LIB) adder.o adder2.o foo.o

adder.o: adder.cpp adder.hpp
	g++ -c -fPIC -g -Wall -o adder.o -I.. adder.cpp

adder2.o: adder2.cpp adder2.hpp
	g++ -c -fPIC -g -Wall -o adder2.o -I.. adder2.cpp

foo.o: foo.cpp foo.hpp
	g++ -c -fPIC -g -Wall -o foo.o -I.. foo.cpp

clean:
	rm -f $(LIB) *.o

