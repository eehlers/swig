
%feature("rp:group", "inheritance");
%feature("rp:include") %{
#include <cl/inheritance.hpp>
%}

namespace ComplexLib {

    // One base class, one derived.

    class Base {
    public:
        Base();
        virtual std::string f();
    };

    class Derived : public Base {
    public:
        Derived();
        virtual std::string f();
    };

    // Hierarchy of 3 classes.

    class A {
    public:
        virtual std::string f0();
        virtual ~A() {}
    };

    class B : public A {
    public:
        virtual std::string f1();
    };

    class C : public B {
    public:
        C();
    };
}

