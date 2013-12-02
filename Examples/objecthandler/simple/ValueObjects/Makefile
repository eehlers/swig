
IDIR = ../../../../../install/include

LIBHW = ../lib/libValueObjectsHw.so
LIB = ../lib/libValueObjects.so

all: $(LIBHW) $(LIB)

$(LIBHW): vo_hw.o
	g++ -shared -fPIC -g -Wall -o $(LIBHW) -I$(IDIR) vo_hw.o

vo_hw.o: vo_hw.cpp vo_hw.hpp
	g++ -c -fPIC -g -Wall -o vo_hw.o -I$(IDIR) vo_hw.cpp

$(LIB): vo_adder.o vo_foo.o
	g++ -shared -fPIC -g -Wall -o $(LIB) -I$(IDIR) vo_adder.o vo_foo.o

vo_adder.o: vo_adder.cpp vo_adder.hpp
	g++ -c -fPIC -g -Wall -o vo_adder.o -I$(IDIR) vo_adder.cpp

vo_foo.o: vo_foo.cpp vo_foo.hpp
	g++ -c -fPIC -g -Wall -o vo_foo.o -I$(IDIR) vo_foo.cpp

clean:
	rm -f $(LIBHW) $(LIB)

