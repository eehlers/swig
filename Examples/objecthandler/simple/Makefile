
all: Library ValueObjects AddinObjects AddinCpp Main

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
