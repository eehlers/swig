
IDIR = $(INSTALL_DIR)/include
LIB = libValueObjects.a

all: $(LIB)

$(LIB): vo_adder.o
	ar rcs $(LIB) vo_adder.o

vo_adder.o: vo_adder.cpp vo_adder.hpp
	g++ -c -fPIC -g -Wall -Wno-unused-local-typedefs -o vo_adder.o -I../.. -I$(IDIR) vo_adder.cpp

clean:
	rm -f $(LIB) *.o

