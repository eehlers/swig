
LIB = ../lib/libSimpleLib.so

all: $(LIB)

$(LIB): simplelib.cpp simplelib.hpp
	g++ -shared -fPIC -g -Wall -o $(LIB) simplelib.cpp

clean:
	rm -f $(LIB)

