
#SWIG = $(REPOSIT_DIR)/install/bin/swig
SWIG = ../../../../preinst-swig
#SWIG_DEBUG = -debug-top 4 -debug-tmsearch

.PHONY: SWIG
SWIG: simple.i
	$(SWIG) $(SWIG_DEBUG) -c++ -reposit -gencpp -prefix sl simple.i

