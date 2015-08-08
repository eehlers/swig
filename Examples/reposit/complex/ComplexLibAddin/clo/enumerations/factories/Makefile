
#IDIR = ../../../../../../install/include
IDIR = /home/countify/install/include
LIB = libEnumerationFactories.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I$(IDIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

