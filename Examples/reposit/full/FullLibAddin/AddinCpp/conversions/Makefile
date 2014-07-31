
IDIR = ../../../../../../install/include
ROOT_DIR=../..
LIB = libConversions.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I$(ROOT_DIR) -I$(IDIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

