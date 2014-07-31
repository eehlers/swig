
SWIG = ../../../../../swig
SWIG_LIB = ../../../../../Lib/reposit
#SWIG_DEBUG = -debug-top 4

.PHONY: SWIG
SWIG: full.i
	$(SWIG) $(SWIG_DEBUG) -I$(SWIG_LIB) -I../.. -c++ -reposit -prefix fl full.i

