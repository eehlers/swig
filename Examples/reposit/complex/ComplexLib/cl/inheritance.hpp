
#ifndef complex_lib_inheritance_hpp
#define complex_lib_inheritance_hpp

// Test inheritance and polymorphism.

#include <iostream>

namespace ComplexLib {

    class A {
    public:
        virtual std::string f();
        virtual ~A() {}
        //virtual ~A()=0;
    };

    class B : public A {};
};

#endif

