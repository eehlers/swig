
IDIR = ../../../../../../install/include
LDIR = ../../../../../../install/lib
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: mainComplexLibAddin

mainComplexLibAddin: $(OBJECTS) ../../lib/libAddinCpp.so
	g++ -g -Wall $(OBJECTS) -o mainComplexLibAddin -L../../lib -L$(LDIR) -lAddinCpp -lObjectHandler

%.o: %.cpp test_all.hpp
	g++ -c -g -Wall -I. -I.. -I$(IDIR) $< -o $@

clean:
	rm -f mainComplexLibAddin $(OBJECTS)

