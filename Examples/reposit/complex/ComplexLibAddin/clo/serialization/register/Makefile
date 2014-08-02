
IDIR = ../../../../../../../../install/include
LIB = libSerializationRegister.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp
	g++ -c -fPIC -g -Wall -I$(IDIR) -I../../.. -I../../../.. $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

