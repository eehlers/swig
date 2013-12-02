
IDIR = ../../../../../install
LDIR = ../../../../../install/lib

all: mainSimpleLib mainSimpleAddinHw mainSimpleAddin

mainSimpleLib: mainSimpleLib.cpp ../lib/libSimpleLib.so
	g++ -g -Wall -o mainSimpleLib mainSimpleLib.cpp -I.. -L../lib -lSimpleLib

mainSimpleAddinHw: mainSimpleAddinHw.cpp ../lib/libAddinCppHw.so
	g++ -fPIC mainSimpleAddinHw.cpp -o mainSimpleAddinHw -I.. -L../lib -lSimpleLib -lValueObjectsHw -lAddinObjectsHw -lAddinCppHw -L$(LDIR) -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

mainSimpleAddin: mainSimpleAddin.cpp ../lib/libAddinCpp.so
	g++ -fPIC mainSimpleAddin.cpp -o mainSimpleAddin -I.. -L../lib -lSimpleLib -lValueObjects -lAddinObjects -lAddinCpp -L$(LDIR) -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

clean:
	rm -f mainSimpleLib mainSimpleAddinHw mainSimpleAddin

