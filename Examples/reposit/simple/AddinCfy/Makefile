
IDIR = $(INSTALL_DIR)/include
SYS_LIBS = -L$(INSTALL_DIR)/lib -lreposit -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem
LIB = ../lib/libCountifyRepositSimpleLib.so

all: $(LIB)

$(LIB): cfy_adder.o init.o
	g++ -shared -fPIC -Wall -Wno-unused-local-typedefs -Wl,--no-as-needed -o $(LIB) cfy_adder.o init.o -L../lib -lSimpleLib -lAddinObjects $(SYS_LIBS)

init.o: init.cpp init.hpp
	g++ -c -fPIC -Wall -Wno-unused-local-typedefs -o init.o -I.. -I$(IDIR) init.cpp

cfy_adder.o: cfy_adder.cpp cfy_adder.hpp
	g++ -c -fPIC -Wall -Wno-unused-local-typedefs -o cfy_adder.o -I.. -I$(IDIR) cfy_adder.cpp

clean:
	rm -f $(LIB) *.o

