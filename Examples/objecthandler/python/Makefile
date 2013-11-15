
SWIG = ../../../preinst-swig

all: _mymodule.so

example_wrap.cxx:
	$(SWIG) -c++ -python -I../simple example.i

example_wrap.o: example_wrap.cxx
	g++ -fPIC -c example_wrap.cxx -I/usr/include/python2.7 -I../simple

_mymodule.so: example_wrap.o
	g++ -shared example_wrap.o -o _mymodule.so -L../simple -lsimple

clean:
	rm -f example_wrap.cxx example_wrap.o mymodule.py mymodule.pyc _mymodule.pyd _mymodule.so

