
IDIR = ../../../../../install

all: mainSimpleLib mainSimpleAddinHw mainSimpleAddin

mainSimpleLib: mainSimpleLib.cpp ../lib/libSimpleLib.so
	g++ -g -Wall -o mainSimpleLib mainSimpleLib.cpp -I.. -L../lib -lSimpleLib

mainSimpleAddinHw: mainSimpleAddinHw.cpp ../lib/libAddinCppHw.so
	g++ -fPIC mainSimpleAddinHw.cpp -o mainSimpleAddinHw -I.. -L../lib -lAddinCppHw -L$(IDIR)/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

mainSimpleAddin: mainSimpleAddin.cpp ../lib/libAddinCpp.so
	g++ -fPIC mainSimpleAddin.cpp -o mainSimpleAddin -I.. -L../lib -lAddinCpp -L$(IDIR)/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

clean:
	rm -f mainSimpleLib mainSimpleAddinHw mainSimpleAddin

