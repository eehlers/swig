
SWIG = ../../../swig
SWIG_LIB = ../../../Lib/reposit
#SWIG_DEBUG = -debug-top 4

all: SWIG Library AddinObjects AddinCpp Main

.PHONY: SWIG
SWIG: simple.i
	$(SWIG) $(SWIG_DEBUG) -I$(SWIG_LIB) -c++ -reposit -prefix sl simple.i

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
