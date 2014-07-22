
IDIR = ../../../../../install/include

LIBHW = ../lib/libValueObjectsHw.so
LIB = ../lib/libValueObjects.so

all: $(LIBHW) $(LIB)

$(LIBHW): vo_hw_adder.o
	g++ -shared -fPIC -g -Wall -o $(LIBHW) -I$(IDIR) vo_hw_adder.o

vo_hw_adder.o: vo_hw_adder.cpp vo_hw_adder.hpp
	g++ -c -fPIC -g -Wall -o vo_hw_adder.o -I$(IDIR) vo_hw_adder.cpp

$(LIB): vo_adder.o
	g++ -shared -fPIC -g -Wall -o $(LIB) -I$(IDIR) vo_adder.o

vo_adder.o: vo_adder.cpp vo_adder.hpp
	g++ -c -fPIC -g -Wall -o vo_adder.o -I$(IDIR) vo_adder.cpp

clean:
	rm -f $(LIBHW) $(LIB) *.o

