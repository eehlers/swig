
IDIR = ../../../../../../install/include
LDIR = ../../../../../../install/lib

all: mainComplexLibAddin

mainComplexLibAddin: mainComplexLibAddin.cpp ../../lib/libAddinCpp.so
	g++ -Wall mainComplexLibAddin.cpp -o mainComplexLibAddin -I.. -I$(IDIR) -L../../lib -L$(LDIR) -lAddinCpp -lObjectHandler

clean:
	rm -f mainComplexLibAddin

