
%module demo

%define MACRO1(T...)
%typemap(tm1) Wrapper<T> "TYPE=T";
%enddef

%typemap(tm1) SWIGTYPE "NO MATCH";

template<class T> class Wrapper {
  MACRO1(T)
};

%template(temp1) Wrapper<A>;

void f0(Wrapper<A> x);
void f1(Wrapper<B> x);
