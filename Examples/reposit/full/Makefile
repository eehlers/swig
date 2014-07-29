
SWIG = ../../../swig
SWIG_LIB = ../../../Lib/reposit
#SWIG_DEBUG = -debug-top 4

all: Library AddinObjects AddinCpp Main

.PHONY: SWIG
SWIG: full.i ../../../swig
	$(SWIG) $(SWIG_DEBUG) -I$(SWIG_LIB) -c++ -reposit full.i

.PHONY: Library
Library:
	$(MAKE) -C Library

.PHONY: AddinObjects
AddinObjects:
	$(MAKE) -C AddinObjects

.PHONY: AddinCpp
AddinCpp:
	$(MAKE) -C AddinCpp

.PHONY: Main
Main:
	$(MAKE) -C Main

clean :
	$(MAKE) -C Library clean
	$(MAKE) -C AddinObjects clean
	$(MAKE) -C AddinCpp clean
	$(MAKE) -C Main clean
