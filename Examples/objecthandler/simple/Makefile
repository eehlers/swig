
SWIG = ../../../preinst-swig
IDIR = ../../../../install

all: mainSimple mainSimpleAddinTest mainSimpleAddin

libsimple.so: simple.cpp simple.hpp
	g++ -shared -fPIC -g -Wall -o libsimple.so simple.cpp

mainSimple: mainSimple.cpp libsimple.so
	g++ -g -Wall -o mainSimple mainSimple.cpp -L. -lsimple

libSimpleAddinTest.so: simpleAddinTest.cpp simpleAddinTest.hpp
	g++ -shared -fPIC -g -Wall -o libSimpleAddinTest.so simpleAddinTest.cpp -L. -lsimple -I$(IDIR)/include -L$(IDIR)/lib -lObjectHandler

mainSimpleAddinTest: mainSimpleAddinTest.cpp libSimpleAddinTest.so
	g++ -fPIC mainSimpleAddinTest.cpp -o mainSimpleAddinTest -L. -lsimple -lSimpleAddinTest

SimpleAddin.cpp: simple.i libsimple.so ../../../swig
	$(SWIG) -prefix oh -c++ -objecthandler simple.i

SimpleAddinXL.cpp: SimpleAddin.cpp

libSimpleAddin.so: SimpleAddin.cpp SimpleAddin.hpp
	g++ -shared -fPIC -g -Wall -o libSimpleAddin.so SimpleAddin.cpp -L. -lsimple -I$(IDIR)/include -L$(IDIR)/lib -lObjectHandler

mainSimpleAddin: mainSimpleAddin.cpp libSimpleAddin.so
	g++ -fPIC mainSimpleAddin.cpp -o mainSimpleAddin -L. -lsimple -lSimpleAddin

clean:
	rm -f *.so mainSimple mainSimpleAddin mainSimpleAddinTest SimpleAddin.?pp SimpleAddinXL.cpp

