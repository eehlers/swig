
IDIR = $(INSTALL_DIR)/include
LIB = libEnumerationFactories.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -Wno-unused-local-typedefs -I$(IDIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

