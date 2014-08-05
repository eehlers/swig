
IDIR = ../../../../../../install/include
LIB = ../../lib/libAddinObjects.so
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
STATIC_LIBS = \
    conversions/libConversions.a \
    enumerations/constructors/libEnumerationConstructors.a \
    enumerations/factories/libEnumerationFactories.a \
    enumerations/register/libEnumerationRegister.a \
    serialization/libSerialization.a \
    serialization/create/libSerializationCreate.a \
    serialization/register/libSerializationRegister.a \
    valueobjects/libValueObjects.a

all: $(LIB)

$(LIB): $(OBJECTS) SubDirs
	g++ -shared -fPIC -g -Wall -o $(LIB) $(OBJECTS) -L../../lib -lComplexLib -Wl,-whole-archive $(STATIC_LIBS) -Wl,-no-whole-archive

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I$(IDIR) -I. -I.. -I../../ComplexLib $< -o $@

.PHONY: SubDirs
SubDirs:
	$(MAKE) -C conversions
	$(MAKE) -C enumerations
	$(MAKE) -C serialization
	$(MAKE) -C valueobjects

clean:
	rm -f $(LIB) $(OBJECTS)
	$(MAKE) -C conversions clean
	$(MAKE) -C enumerations clean
	$(MAKE) -C serialization clean
	$(MAKE) -C valueobjects clean

