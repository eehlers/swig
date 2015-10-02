
%module demo

%define SWIG_STD_VECTOR_ENHANCED(CTYPE...)
%typemap(foo) const boost::shared_ptr<CTYPE> & %{
XXXXXXXXXXXXXXX
CTYPE
XXXXXXXXXXXXXXX
%}
%enddef

%typemap(foo) SWIGTYPE "NO MATCH";

SWIG_STD_VECTOR_ENHANCED(Calendar)
SWIG_STD_VECTOR_ENHANCED(DayCounter)

void f0(const boost::shared_ptr<Calendar> &x);
void f1(const boost::shared_ptr<DayCounter> &x);
