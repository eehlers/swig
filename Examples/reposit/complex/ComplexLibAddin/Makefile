
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
	$(MAKE) -C docstrings
	$(MAKE) -C swig

.PHONY: Docs
Docs:
	$(MAKE) -C Docs

