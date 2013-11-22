
LIB = ../lib/libAddinObjectsHw.so
IDIR = ../../../../../install/include

all: $(LIB)

$(LIB): obj_hw.cpp obj_hw.hpp
	g++ -shared -fPIC -g -Wall -o $(LIB) -I. -I.. -I$(IDIR) obj_hw.cpp

clean:
	rm -f $(LIB)

