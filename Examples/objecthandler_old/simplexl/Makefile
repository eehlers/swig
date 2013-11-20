
SWIG = ../../../preinst-swig
IDIR = ../../../../install

all: SimpleAddinXL.cpp

SimpleAddinXL.cpp: ../simple/simple.i ../../../swig
	$(SWIG) -prefix oh -c++ -objecthandler ../simple/simple.i

clean:
	rm -f SimpleAddinXL.cpp

