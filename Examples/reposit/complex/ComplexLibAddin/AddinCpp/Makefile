
IDIR = $(INSTALL_DIR)/include
SYS_LIBS = -L$(INSTALL_DIR)/lib -lreposit -lboost_regex -lboost_serialization -lboost_system -lboost_filesystem
LIB = ../../lib/libAddinCpp.so
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
#STATIC_LIBS = conversions/libConversions.a

all: $(LIB)

#$(LIB): $(OBJECTS) Conversions
#	g++ -shared -fPIC -g -Wall -Wno-unused-local-typedefs -o $(LIB) $(OBJECTS) $(STATIC_LIBS) -L../../lib -lComplexLib -lAddinObjects $(SYS_LIBS)

$(LIB): $(OBJECTS)
	g++ -shared -fPIC -g -Wall -Wno-unused-local-typedefs -o $(LIB) $(OBJECTS) -L../../lib -lComplexLib -lAddinObjects $(SYS_LIBS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -Wno-unused-local-typedefs -I$(IDIR) -I. -I.. -I../../ComplexLib $< -o $@

#.PHONY: Conversions
#Conversions:
#	$(MAKE) -C conversions

clean:
	rm -f $(LIB) $(OBJECTS)
	rm -f add_*.?pp
#	$(MAKE) -C conversions clean

