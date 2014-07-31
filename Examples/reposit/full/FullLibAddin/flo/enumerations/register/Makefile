
IDIR = ../../../../../../../../install/include
LIB = libEnumerationRegister.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I$(IDIR) -I../../.. -I../../../../FullLib $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

