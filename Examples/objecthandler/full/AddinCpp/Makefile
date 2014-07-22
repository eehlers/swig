
IDIR = ../../../../../install/include
SYS_LIBS = -L../../../../../install/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

LIB = ../lib/libAddinCpp.so

all: $(LIB)

$(LIB): cpp_adder.o cpp_adder2.o cpp_foo.o conversions.o
	g++ -shared -fPIC -g -Wall -o $(LIB) -I.. -I$(IDIR) cpp_adder.o cpp_adder2.o cpp_foo.o conversions.o -L../lib -lSimpleLib -lValueObjects -lAddinObjects $(SYS_LIBS)

conversions.o: conversions.cpp
	g++ -c -fPIC -g -Wall -o conversions.o -I.. -I$(IDIR) conversions.cpp

cpp_adder.o: cpp_adder.cpp cpp_adder.hpp
	g++ -c -fPIC -g -Wall -o cpp_adder.o -I.. -I$(IDIR) cpp_adder.cpp

cpp_adder2.o: cpp_adder2.cpp cpp_adder2.hpp
	g++ -c -fPIC -g -Wall -o cpp_adder2.o -I.. -I$(IDIR) cpp_adder2.cpp

cpp_foo.o: cpp_foo.cpp cpp_foo.hpp
	g++ -c -fPIC -g -Wall -o cpp_foo.o -I.. -I$(IDIR) cpp_foo.cpp

clean:
	rm -f $(LIB) *.o

