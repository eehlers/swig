
IDIR = ../../../../../install/include
LDIR = ../../../../../install/lib

LIBHW = ../lib/libAddinCppHw.so
LIB = ../lib/libAddinCpp.so

all: $(LIBHW) $(LIB)

$(LIBHW): cpp_hw.cpp cpp_hw.hpp
	g++ -shared -fPIC -g -Wall -o $(LIBHW) -I.. -I$(IDIR) cpp_hw.cpp -L../lib -lValueObjectsHw -lAddinObjectsHw -lSimpleLib -L$(LDIR) -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

$(LIB): cpp_adder.cpp cpp_adder.hpp
	g++ -shared -fPIC -g -Wall -o $(LIB) -I.. -I$(IDIR) cpp_adder.cpp -L../lib -lValueObjects -lAddinObjects -lSimpleLib -L$(LDIR) -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

clean:
	rm -f $(LIBHW) $(LIB)

