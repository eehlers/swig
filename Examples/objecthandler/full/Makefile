
SWIG = ../../../preinst-swig
#SWIG_DEBUG = -debug-top 4

all: SWIG Library ValueObjects AddinObjects AddinCpp Main

.PHONY: SWIG
SWIG: simple.i ../../../swig
	$(SWIG) $(SWIG_DEBUG) -prefix sl -c++ -objecthandler simple.i

.PHONY: Library
Library:
	$(MAKE) -C Library

.PHONY: ValueObjects
ValueObjects:
	$(MAKE) -C ValueObjects

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
	$(MAKE) -C ValueObjects clean
	$(MAKE) -C AddinObjects clean
	$(MAKE) -C AddinCpp clean
	$(MAKE) -C Main clean
