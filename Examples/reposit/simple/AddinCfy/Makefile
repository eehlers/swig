
#IDIR = ../../../../../install/include
IDIR = /home/countify/install/include
#SYS_LIBS = -L../../../../../install/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem
SYS_LIBS = -L/home/countify/install/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem
LIB = ../lib/libAddinCfy.so

all: $(LIB)

$(LIB): cfy_adder.o init.o
	g++ -shared -fPIC -Wall -Wl,--no-as-needed -o $(LIB) cfy_adder.o init.o -L../lib -lSimpleLib -lAddinObjects $(SYS_LIBS)

init.o: init.cpp init.hpp
	g++ -c -fPIC -Wall -o init.o -I.. -I$(IDIR) init.cpp

cfy_adder.o: cfy_adder.cpp cfy_adder.hpp
	g++ -c -fPIC -Wall -o cfy_adder.o -I.. -I$(IDIR) cfy_adder.cpp

clean:
	rm -f $(LIB) *.o

