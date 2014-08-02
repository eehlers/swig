
IDIR = ../../../../../../../../install/include
LIB = libSerializationCreate.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp
	g++ -c -fPIC -g -Wall -I$(IDIR) -I../../.. -I../../../../ComplexLib $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

