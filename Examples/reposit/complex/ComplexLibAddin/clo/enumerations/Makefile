
all: SubDirs

.PHONY: SubDirs
SubDirs:
	$(MAKE) -C constructors
	$(MAKE) -C factories
	$(MAKE) -C register

clean:
	rm -f $(LIB)
	$(MAKE) -C constructors clean
	$(MAKE) -C factories clean
	$(MAKE) -C register clean

