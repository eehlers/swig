
IDIR = ../../../../../install/include
SYS_LIBS = -L../../../../../install/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem
LIB = ../lib/libAddinCpp.so

all: $(LIB)

$(LIB): cpp_adder.o init.o
	g++ -shared -fPIC -Wall -o $(LIB) cpp_adder.o init.o -L../lib -lSimpleLib -lAddinObjects $(SYS_LIBS)

init.o: init.cpp init.hpp
	g++ -c -fPIC -Wall -o init.o -I.. -I$(IDIR) init.cpp

cpp_adder.o: cpp_adder.cpp cpp_adder.hpp
	g++ -c -fPIC -Wall -o cpp_adder.o -I.. -I$(IDIR) cpp_adder.cpp

clean:
	rm -f $(LIB) *.o

