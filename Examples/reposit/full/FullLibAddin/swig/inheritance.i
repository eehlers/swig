
%feature("rp:group", "inheritance");
%feature("rp:include") %{
#include "FullLib/inheritance.hpp"
%}

namespace FullLib {
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

