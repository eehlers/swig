
IDIR = ../../../../../install/include
SYS_LIBS = -L../../../../../install/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

LIBHW = ../lib/libAddinCppHw.so
LIB = ../lib/libAddinCpp.so

all: $(LIBHW) $(LIB)

$(LIBHW): cpp_hw_adder.o
	g++ -shared -fPIC -g -Wall -o $(LIBHW) -I.. -I$(IDIR) cpp_hw_adder.o -L../lib -lSimpleLib -lValueObjectsHw -lAddinObjectsHw $(SYS_LIBS)

cpp_hw_adder.o: cpp_hw_adder.cpp cpp_hw_adder.hpp
	g++ -c -fPIC -g -Wall -o cpp_hw_adder.o -I.. -I$(IDIR) cpp_hw_adder.cpp

$(LIB): cpp_adder.o
	g++ -shared -fPIC -g -Wall -o $(LIB) -I.. -I$(IDIR) cpp_adder.o -L../lib -lSimpleLib -lValueObjects -lAddinObjects $(SYS_LIBS)

cpp_adder.o: cpp_adder.cpp cpp_adder.hpp
	g++ -c -fPIC -g -Wall -o cpp_adder.o -I.. -I$(IDIR) cpp_adder.cpp

clean:
	rm -f $(LIBHW) $(LIB) *.o

