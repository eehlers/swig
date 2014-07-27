
IDIR = ../../../../../install/include
LIBHW = ../lib/libAddinObjectsHw.so
LIB = ../lib/libAddinObjects.so
STATIC_LIBS = -Wl,-whole-archive valueobjects/libValueObjects.a -Wl,-no-whole-archive
STATIC_LIBS_HW = -Wl,-whole-archive valueobjects/libValueObjectsHw.a -Wl,-no-whole-archive

all: $(LIBHW) $(LIB)

$(LIBHW): obj_hw_adder.o ValueObjects
	g++ -shared -fPIC -g -Wall -o $(LIBHW) obj_hw_adder.o $(STATIC_LIBS_HW)

obj_hw_adder.o: obj_hw_adder.cpp obj_hw_adder.hpp
	g++ -c -fPIC -g -Wall -o obj_hw_adder.o -I. -I.. -I$(IDIR) obj_hw_adder.cpp

$(LIB): obj_adder.o ValueObjects
	g++ -shared -fPIC -g -Wall -o $(LIB) -I. -I.. -I$(IDIR) obj_adder.o $(STATIC_LIBS)

obj_adder.o: obj_adder.cpp obj_adder.hpp
	g++ -c -fPIC -g -Wall -o obj_adder.o -I. -I.. -I$(IDIR) obj_adder.cpp

.PHONY: ValueObjects
ValueObjects:
	$(MAKE) -C valueobjects

clean:
	rm -f $(LIBHW) $(LIB) *.o
	$(MAKE) -C valueobjects clean

