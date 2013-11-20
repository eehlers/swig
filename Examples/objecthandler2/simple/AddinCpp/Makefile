
LIB = ../lib/libAddinCpp.so
IDIR = ../../../../../install/include

all: $(LIB)

$(LIB): sla_hw.cpp sla_hw.hpp
	g++ -shared -fPIC -g -Wall -o $(LIB) -I.. -I$(IDIR) sla_hw.cpp -L../lib -lValueObjects -lAddinObjects -lSimpleLib

clean:
	rm -f $(LIB)

