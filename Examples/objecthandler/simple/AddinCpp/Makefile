
IDIR = ../../../../../install/include
SYS_LIBS = -L../../../../../install/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

LIBHW = ../lib/libAddinCppHw.so
LIB = ../lib/libAddinCpp.so

all: $(LIBHW) $(LIB)

$(LIBHW): cpp_hw.o
	g++ -shared -fPIC -g -Wall -o $(LIBHW) -I.. -I$(IDIR) cpp_hw.o -L../lib -lSimpleLib -lValueObjectsHw -lAddinObjectsHw $(SYS_LIBS)

cpp_hw.o: cpp_hw.cpp cpp_hw.hpp
	g++ -c -fPIC -g -Wall -o cpp_hw.o -I.. -I$(IDIR) cpp_hw.cpp

$(LIB): cpp_adder.o cpp_foo.o
	g++ -shared -fPIC -g -Wall -o $(LIB) -I.. -I$(IDIR) cpp_adder.o cpp_foo.o -L../lib -lSimpleLib -lValueObjects -lAddinObjects $(SYS_LIBS)

cpp_adder.o: cpp_adder.cpp cpp_adder.hpp
	g++ -c -fPIC -g -Wall -o cpp_adder.o -I.. -I$(IDIR) cpp_adder.cpp

cpp_foo.o: cpp_foo.cpp cpp_foo.hpp
	g++ -c -fPIC -g -Wall -o cpp_foo.o -I.. -I$(IDIR) cpp_foo.cpp

clean:
	rm -f $(LIBHW) $(LIB) *.o

