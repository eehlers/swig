
IDIR = $(INSTALL_DIR)/include
LIB = libEnumerationConstructors.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I../../.. -I../../../../ComplexLib -I$(IDIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

