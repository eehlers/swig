
IDIR = $(INSTALL_DIR)/include
LIB = libSerializationCreate.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp
	g++ -c -fPIC -g -Wall -Wno-unused-local-typedefs -I$(IDIR) -I../../.. -I../../../../ComplexLib $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)
	rm -f create_*.?pp

