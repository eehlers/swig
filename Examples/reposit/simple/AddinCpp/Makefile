
IDIR = $(INSTALL_DIR)/include
SYS_LIBS = -L$(INSTALL_DIR)/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem
LIB = ../lib/libAddinCpp.so

all: $(LIB)

$(LIB): add_adder.o init.o
	g++ -shared -fPIC -Wall -o $(LIB) add_adder.o init.o -L../lib -lSimpleLib -lAddinObjects $(SYS_LIBS)

init.o: init.cpp init.hpp
	g++ -c -fPIC -Wall -o init.o -I.. -I$(IDIR) init.cpp

add_adder.o: add_adder.cpp add_adder.hpp
	g++ -c -fPIC -Wall -o add_adder.o -I.. -I$(IDIR) add_adder.cpp

clean:
	rm -f $(LIB) *.o

