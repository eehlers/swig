
#ifndef complex_lib_inheritance_hpp
#define complex_lib_inheritance_hpp

// Test inheritance and polymorphism.

#include <string>

namespace ComplexLib {

    // One base class, one derived.

    class Base {
    public:
        virtual std::string f()=0;
        virtual ~Base() {}
    };

    class Derived : public Base {
    public:
        virtual std::string f() { return "ComplexLib::Derived::f()"; }
    };

    // Hierarchy of 3 classes.

    class A {
    public:
        virtual std::string f0()=0;
        virtual ~A() {}
    };

    class B : public A {
    public:
        virtual std::string f1()=0;
    };

    class C : public B {
    public:
        virtual std::string f0() { return "ComplexLib::C::f0()"; }
        virtual std::string f1() { return "ComplexLib::C::f1()"; }
    };

};

#endif

