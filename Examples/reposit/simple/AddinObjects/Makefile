
IDIR = ../../../../../install/include
LIB = ../lib/libAddinObjects.so
STATIC_LIBS = -Wl,-whole-archive valueobjects/libValueObjects.a -Wl,-no-whole-archive

all: $(LIB)

$(LIB): obj_adder.o ValueObjects
	g++ -shared -fPIC -g -Wall -o $(LIB) -I. -I.. -I$(IDIR) obj_adder.o $(STATIC_LIBS)

obj_adder.o: obj_adder.cpp obj_adder.hpp
	g++ -c -fPIC -g -Wall -o obj_adder.o -I. -I.. -I$(IDIR) obj_adder.cpp

.PHONY: ValueObjects
ValueObjects:
	$(MAKE) -C valueobjects

clean:
	rm -f $(LIB) *.o
	$(MAKE) -C valueobjects clean

