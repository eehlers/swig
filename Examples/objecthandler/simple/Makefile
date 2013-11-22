
SWIG = ../../../preinst-swig

all: SimpleAddin.cpp Library ValueObjects AddinObjects AddinCpp Main

SimpleAddin.cpp: simple.i ../../../swig
	$(SWIG) -prefix sl -c++ -objecthandler simple.i

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
