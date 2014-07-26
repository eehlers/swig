
IDIR = ../../../../../install/include
LIB = ../lib/libAddinObjects.so
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
STATIC_LIBS = \
    enumerations/constructors/libEnumerationConstructors.a \
    enumerations/factories/libEnumerationFactories.a \
    enumerations/register/libEnumerationRegister.a \
    valueobjects/libValueObjects.a

all: $(LIB)

$(LIB): $(OBJECTS) SubDirs
	g++ -shared -fPIC -g -Wall -o $(LIB) $(OBJECTS) -Wl,-whole-archive $(STATIC_LIBS) -Wl,-no-whole-archive

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I. -I.. -I$(IDIR) $< -o $@

.PHONY: SubDirs
SubDirs:
	$(MAKE) -C enumerations
	$(MAKE) -C valueobjects

clean:
	rm -f $(LIB) $(OBJECTS)
	$(MAKE) -C enumerations
	$(MAKE) -C valueobjects

