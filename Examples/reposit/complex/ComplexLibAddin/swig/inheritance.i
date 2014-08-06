
%feature("rp:group", "inheritance");
%feature("rp:include") %{
#include <cl/inheritance.hpp>
%}

namespace ComplexLib {

    class A {
    public:
        A();
        std::string f();
    };

    class B {
    public:
        virtual std::string f();
    };

    class C : public B {
    public:
        C();
    };
}

