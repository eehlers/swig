
IDIR = $(INSTALL_DIR)/include
LIB = ../lib/libAddinObjects.so
STATIC_LIBS = -Wl,-whole-archive valueobjects/libValueObjects.a -Wl,-no-whole-archive

all: $(LIB)

$(LIB): ValueObjects
	g++ -shared -fPIC -g -Wall -Wno-unused-local-typedefs -o $(LIB) -I. -I.. -I$(IDIR) $(STATIC_LIBS)

.PHONY: ValueObjects
ValueObjects:
	$(MAKE) -C valueobjects

clean:
	rm -f $(LIB) *.o
	$(MAKE) -C valueobjects clean

