
IDIR = ../../../../../install/include

LIB = ../lib/libValueObjects.so

all: $(LIB)

$(LIB): vo_adder.o vo_adder2.o vo_foo.o
	g++ -shared -fPIC -g -Wall -o $(LIB) -I$(IDIR) vo_adder.o vo_adder2.o vo_foo.o

vo_adder.o: vo_adder.cpp vo_adder.hpp
	g++ -c -fPIC -g -Wall -o vo_adder.o -I$(IDIR) vo_adder.cpp

vo_adder2.o: vo_adder2.cpp vo_adder2.hpp
	g++ -c -fPIC -g -Wall -o vo_adder2.o -I$(IDIR) vo_adder2.cpp

vo_foo.o: vo_foo.cpp vo_foo.hpp
	g++ -c -fPIC -g -Wall -o vo_foo.o -I$(IDIR) vo_foo.cpp

clean:
	rm -f $(LIB) *.o

