
#SWIG = $(REPOSIT_DIR)/install/bin/swig
SWIG = ../../../preinst-swig
#SWIG_DEBUG = -debug-top 4 -debug-tmsearch

all: Library AddinObjects AddinCpp Main

.PHONY: SWIG
SWIG:
	$(MAKE) -C swig

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

