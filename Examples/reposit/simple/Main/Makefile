
IDIR = ../../../../../install/include
LDIR = ../../../../../install/lib

all: mainSimpleLib mainSimpleAddinHw mainSimpleAddin

mainSimpleLib: mainSimpleLib.cpp ../lib/libSimpleLib.so
	g++ -Wall mainSimpleLib.cpp -o mainSimpleLib -I.. -L../lib -lSimpleLib

mainSimpleAddinHw: mainSimpleAddinHw.cpp ../lib/libAddinCppHw.so
	g++ -Wall mainSimpleAddinHw.cpp -o mainSimpleAddinHw -I.. -I$(IDIR) -L../lib -lAddinCppHw

mainSimpleAddin: mainSimpleAddin.cpp ../lib/libAddinCpp.so
	g++ -Wall mainSimpleAddin.cpp -o mainSimpleAddin -I.. -I$(IDIR) -L../lib -lAddinCpp

clean:
	rm -f mainSimpleLib mainSimpleAddinHw mainSimpleAddin

