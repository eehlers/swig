
all: SubDirs

.PHONY: SubDirs
SubDirs:
	$(MAKE) -C flo
	$(MAKE) -C AddinCpp
	$(MAKE) -C Main

clean :
	$(MAKE) -C flo clean
	$(MAKE) -C AddinCpp clean
	$(MAKE) -C Main clean

.PHONY: SWIG
SWIG:
	$(MAKE) -C swig SWIG

