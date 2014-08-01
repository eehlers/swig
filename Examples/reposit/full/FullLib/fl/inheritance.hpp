
#ifndef full_lib_inheritance_hpp
#define full_lib_inheritance_hpp

// Test inheritance and polymorphism.

#include <iostream>

namespace FullLib {

    class Base {
    public:
        virtual void f() { std::cout << "Base::f()" << std::endl; }
        virtual ~Base() {}
    };

    class Derived : public Base {};
};

#endif

