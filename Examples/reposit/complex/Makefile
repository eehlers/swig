
all: SubDirs

.PHONY: SubDirs
SubDirs:
	$(MAKE) -C ComplexLib
	$(MAKE) -C ComplexLibAddin

clean :
	$(MAKE) -C ComplexLib clean
	$(MAKE) -C ComplexLibAddin clean

.PHONY: SWIG
SWIG:
	$(MAKE) -C ComplexLibAddin SWIG

.PHONY: Docs
Docs:
	$(MAKE) -C ComplexLibAddin Docs

