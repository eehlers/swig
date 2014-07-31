
SWIG = ../../../../../swig
SWIG_LIB = ../../../../../Lib/reposit
#SWIG_DEBUG = -debug-top 4

.PHONY: SWIG
SWIG: full.i ../../../swig
	$(SWIG) $(SWIG_DEBUG) -I$(SWIG_LIB) -c++ -reposit -prefix fl full.i

