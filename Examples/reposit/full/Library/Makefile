
IDIR = ../../../../../install/include
LIB = ../lib/libSimpleLib.so
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	g++ -shared -fPIC -g -Wall -o $(LIB) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I.. -I$(IDIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

