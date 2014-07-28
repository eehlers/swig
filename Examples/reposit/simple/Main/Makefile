
IDIR = ../../../../../install/include
LDIR = ../../../../../install/lib

all: mainSimpleLibAddin

mainSimpleAddin: mainSimpleLibAddin.cpp ../lib/libAddinCpp.so
	g++ -Wall mainSimpleLibAddin.cpp -o mainSimpleLibAddin -I.. -I$(IDIR) -L../lib -lAddinCpp

clean:
	rm -f mainSimpleLibAddin

