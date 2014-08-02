
%feature("rp:group", "inheritance");
%feature("rp:include") %{
#include <cl/inheritance.hpp>
%}

namespace ComplexLib {
    class Base {
    public:
        Base();
        void f();
    };
    class Derived : public Base {
    public:
        Derived();
    };
}

