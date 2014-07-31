
IDIR = ../../../../../../install/include
SYS_LIBS = -L../../../../../../install/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem
LIB = ../../lib/libAddinCpp.so
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
STATIC_LIBS = conversions/libConversions.a

all: $(LIB)

$(LIB): $(OBJECTS) Conversions
	g++ -shared -fPIC -g -Wall -o $(LIB) $(OBJECTS) $(STATIC_LIBS) -L../../lib -lFullLib -lValueObjects -lAddinObjects $(SYS_LIBS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I$(IDIR) -I. -I.. -I../../FullLib $< -o $@

.PHONY: Conversions
Conversions:
	$(MAKE) -C conversions

clean:
	rm -f $(LIB) $(OBJECTS)
	$(MAKE) -C conversions clean

