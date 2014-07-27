
IDIR = ../../../../../../install/include

LIBHW = libValueObjectsHw.a
LIB = libValueObjects.a

all: $(LIBHW) $(LIB)

$(LIBHW): vo_hw_adder.o
	ar rcs $(LIBHW) vo_hw_adder.o

vo_hw_adder.o: vo_hw_adder.cpp vo_hw_adder.hpp
	g++ -c -fPIC -g -Wall -o vo_hw_adder.o -I$(IDIR) vo_hw_adder.cpp

$(LIB): vo_adder.o
	ar rcs $(LIB) vo_adder.o

vo_adder.o: vo_adder.cpp vo_adder.hpp
	g++ -c -fPIC -g -Wall -o vo_adder.o -I$(IDIR) vo_adder.cpp

clean:
	rm -f $(LIBHW) $(LIB) *.o

