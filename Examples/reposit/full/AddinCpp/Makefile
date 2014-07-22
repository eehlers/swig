
IDIR = ../../../../../install/include
SYS_LIBS = -L../../../../../install/lib -lObjectHandler -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem
LIB = ../lib/libAddinCpp.so
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	g++ -shared -fPIC -g -Wall -o $(LIB) -I.. -I$(IDIR) $(OBJECTS) -L../lib -lSimpleLib -lValueObjects -lAddinObjects $(SYS_LIBS)

conversions.o: conversions.cpp
	g++ -c -fPIC -g -Wall -o conversions.o -I.. -I$(IDIR) conversions.cpp

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I.. -I$(IDIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

