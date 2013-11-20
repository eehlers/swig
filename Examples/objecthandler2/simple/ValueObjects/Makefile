
LIB = ../lib/libValueObjects.so
IDIR = ../../../../../install/include

all: $(LIB)

$(LIB): vo_hw.cpp vo_hw.hpp
	g++ -shared -fPIC -g -Wall -o $(LIB) -I$(IDIR) vo_hw.cpp

clean:
	rm -f $(LIB)

