
#SWIG = $(REPOSIT_DIR)/install/bin/swig
SWIG = ../../../../../preinst-swig
#SWIG_DEBUG = -debug-top 4 -debug-tmsearch

all:

.PHONY: SWIG
SWIG: complex.i
#	$(SWIG) $(SWIG_DEBUG) -c++ -reposit -gencpp -prefix cl complex.i
	$(SWIG) $(SWIG_DEBUG) -c++ -reposit -gencpp -genxll -prefix cl complex.i

