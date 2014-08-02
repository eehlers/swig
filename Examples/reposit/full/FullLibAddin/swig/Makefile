
#SWIG = $(REPOSIT_DIR)/install/bin/swig
SWIG = ../../../../../preinst-swig
#SWIG_DEBUG = -debug-top 4 -debug-tmsearch

all:

.PHONY: SWIG
SWIG: full.i
	$(SWIG) $(SWIG_DEBUG) -c++ -reposit -prefix fl full.i

