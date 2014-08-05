
#ifndef complex_lib_inheritance_hpp
#define complex_lib_inheritance_hpp

// Test inheritance and polymorphism.

#include <iostream>

namespace ComplexLib {

    class Base {
    public:
        virtual void f() { std::cout << "Base::f()" << std::endl; }
        virtual ~Base() {}
    };

    class Derived : public Base {};

//    class A {
//    public:
//        virtual std::string f();
//        virtual ~A()=0;
//    };
//
//    class B : public A {};
};

#endif

