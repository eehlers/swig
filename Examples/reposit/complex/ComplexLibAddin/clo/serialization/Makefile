
IDIR = ../../../../../../../install/include
LIB = libSerialization.a
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(LIB) SubDirs

$(LIB): $(OBJECTS)
	ar rcs $(LIB) $(OBJECTS)

%.o: %.cpp
	g++ -c -fPIC -g -Wall -I$(IDIR) -I../.. $< -o $@

.PHONY: SubDirs
SubDirs:
	$(MAKE) -C create
	$(MAKE) -C register

clean:
	rm -f $(LIB) $(OBJECTS)
	rm register_creators.cpp
	$(MAKE) -C create clean
	$(MAKE) -C register clean

