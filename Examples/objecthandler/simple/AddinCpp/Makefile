
IDIR = ../../../../../install/include

LIBHW = ../lib/libAddinCppHw.so
LIB = ../lib/libAddinCpp.so

all: $(LIBHW) $(LIB)

$(LIBHW): sla_hw.cpp sla_hw.hpp
	g++ -shared -fPIC -g -Wall -o $(LIBHW) -I.. -I$(IDIR) sla_hw.cpp -L../lib -lValueObjectsHw -lAddinObjectsHw -lSimpleLib

$(LIB): sla.cpp sla.hpp
	g++ -shared -fPIC -g -Wall -o $(LIB) -I.. -I$(IDIR) sla.cpp -L../lib -lValueObjects -lAddinObjects -lSimpleLib

clean:
	rm -f $(LIBHW) $(LIB)

