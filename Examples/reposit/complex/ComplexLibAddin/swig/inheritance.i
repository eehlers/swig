
%feature("rp:group", "inheritance");
%feature("rp:include") %{
#include <cl/inheritance.hpp>
%}

namespace ComplexLib {
    class A {
    public:
        //A();
        virtual std::string f();
    };

    class B : public A {
    public:
        B();
    };
}

