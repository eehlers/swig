
IDIR = ../../../../../../../install/include
ROOT_DIR=../../..
LIB = libEnumerationRegister.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp %.hpp
	g++ -c -fPIC -g -Wall -I$(IDIR) -I$(ROOT_DIR) $< -o $@

clean:
	rm -f $(LIB) $(OBJECTS)

