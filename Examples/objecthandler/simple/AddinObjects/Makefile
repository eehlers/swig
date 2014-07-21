
IDIR = ../../../../../install/include

LIBHW = ../lib/libAddinObjectsHw.so
LIB = ../lib/libAddinObjects.so

all: $(LIBHW) $(LIB)

$(LIBHW): obj_hw_adder.o obj_hw_adder2.o
	g++ -shared -fPIC -g -Wall -o $(LIBHW) -I. -I.. -I$(IDIR) obj_hw_adder.o obj_hw_adder2.o

obj_hw_adder.o: obj_hw_adder.cpp obj_hw_adder.hpp
	g++ -c -fPIC -g -Wall -o obj_hw_adder.o -I. -I.. -I$(IDIR) obj_hw_adder.cpp

obj_hw_adder2.o: obj_hw_adder2.cpp obj_hw_adder2.hpp
	g++ -c -fPIC -g -Wall -o obj_hw_adder2.o -I. -I.. -I$(IDIR) obj_hw_adder2.cpp

$(LIB): obj_adder.o obj_foo.o
	g++ -shared -fPIC -g -Wall -o $(LIB) -I. -I.. -I$(IDIR) obj_adder.o obj_foo.o

obj_adder.o: obj_adder.cpp obj_adder.hpp
	g++ -c -fPIC -g -Wall -o obj_adder.o -I. -I.. -I$(IDIR) obj_adder.cpp

obj_foo.o: obj_foo.cpp obj_foo.hpp
	g++ -c -fPIC -g -Wall -o obj_foo.o -I. -I.. -I$(IDIR) obj_foo.cpp

clean:
	rm -f $(LIBHW) $(LIB) *.o

