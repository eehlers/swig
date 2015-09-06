
IDIR = $(INSTALL_DIR)/include
LIB = libValueObjects.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -Wno-unused-local-typedefs -I../.. -I$(IDIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)
	rm -f vo_*.?pp

