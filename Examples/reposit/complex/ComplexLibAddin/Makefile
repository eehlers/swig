
all: SubDirs

.PHONY: SubDirs
SubDirs:
	$(MAKE) -C clo
	$(MAKE) -C AddinCpp
	$(MAKE) -C Main

clean :
	$(MAKE) -C clo clean
	$(MAKE) -C AddinCpp clean
	$(MAKE) -C Main clean

.PHONY: SWIG
SWIG:
	$(MAKE) -C swig SWIG

.PHONY: SWIGCPP
SWIGCPP:
	$(MAKE) -C swig SWIGCPP

