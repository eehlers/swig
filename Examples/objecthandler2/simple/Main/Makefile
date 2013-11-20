
IDIR = ../../../../../install

all: mainSimpleLib mainSimpleAddinHw

mainSimpleLib: mainSimpleLib.cpp ../lib/libSimpleLib.so
	g++ -g -Wall -o mainSimpleLib mainSimpleLib.cpp -I../Library -L../lib -lSimpleLib

mainSimpleAddinHw: mainSimpleAddinHw.cpp ../lib/libAddinObjects.so
	g++ -fPIC mainSimpleAddinHw.cpp -o mainSimpleAddinHw -I.. -L../lib -lAddinCpp -L$(IDIR)/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem

#mainSimpleAddin: mainSimpleAddin.cpp libSimpleAddin.so
#	g++ -fPIC mainSimpleAddin.cpp -o mainSimpleAddin -L. -lsimple -lSimpleAddin

clean:
	rm -f mainSimpleLib mainSimpleAddinHw

