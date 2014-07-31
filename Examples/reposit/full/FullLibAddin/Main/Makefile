
IDIR = ../../../../../../install/include
LDIR = ../../../../../../install/lib

all: mainFullLibAddin

mainFullLibAddin: mainFullLibAddin.cpp ../../lib/libAddinCpp.so
	g++ -Wall mainFullLibAddin.cpp -o mainFullLibAddin -I.. -I$(IDIR) -L../../lib -L$(LDIR) -lAddinCpp -lObjectHandler

clean:
	rm -f mainFullLibAddin

