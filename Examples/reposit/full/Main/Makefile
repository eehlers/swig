
IDIR = ../../../../../install/include
LDIR = ../../../../../install/lib

all: mainSimpleAddin

mainSimpleAddin: mainSimpleAddin.cpp ../lib/libAddinCpp.so
	g++ -Wall mainSimpleAddin.cpp -o mainSimpleAddin -I.. -I$(IDIR) -L../lib -lAddinCpp

clean:
	rm -f mainSimpleAddin

