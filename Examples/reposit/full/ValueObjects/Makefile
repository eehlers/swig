
IDIR = ../../../../../install/include
LIB = ../lib/libValueObjects.so
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	g++ -shared -fPIC -g -Wall -o $(LIB) -I$(IDIR) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I$(IDIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

