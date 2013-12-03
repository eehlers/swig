
IDIR = ../../../../../install/include

LIBHW = ../lib/libAddinObjectsHw.so
LIB = ../lib/libAddinObjects.so

all: $(LIBHW) $(LIB)

$(LIBHW): obj_hw.o
	g++ -shared -fPIC -g -Wall -o $(LIBHW) -I. -I.. -I$(IDIR) obj_hw.o

obj_hw.o: obj_hw.cpp obj_hw.hpp
	g++ -c -fPIC -g -Wall -o obj_hw.o -I. -I.. -I$(IDIR) obj_hw.cpp

$(LIB): obj_adder.o obj_foo.o
	g++ -shared -fPIC -g -Wall -o $(LIB) -I. -I.. -I$(IDIR) obj_adder.o obj_foo.o

obj_adder.o: obj_adder.cpp obj_adder.hpp
	g++ -c -fPIC -g -Wall -o obj_adder.o -I. -I.. -I$(IDIR) obj_adder.cpp

obj_foo.o: obj_foo.cpp obj_foo.hpp
	g++ -c -fPIC -g -Wall -o obj_foo.o -I. -I.. -I$(IDIR) obj_foo.cpp

clean:
	rm -f $(LIBHW) $(LIB) *.o

