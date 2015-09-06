
IDIR = $(INSTALL_DIR)/include
LDIR = $(INSTALL_DIR)/lib
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: mainComplexLibAddin

mainComplexLibAddin: $(OBJECTS) ../../lib/libAddinCpp.so
	g++ -g -Wall -Wno-unused-local-typedefs $(OBJECTS) -o mainComplexLibAddin -L../../lib -L$(LDIR) -lAddinCpp -lreposit

%.o: %.cpp test_all.hpp
	g++ -c -g -Wall -Wno-unused-local-typedefs -I. -I.. -I$(IDIR) $< -o $@

clean:
	rm -f mainComplexLibAddin $(OBJECTS)

