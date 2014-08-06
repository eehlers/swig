
#ifndef complex_lib_inheritance_hpp
#define complex_lib_inheritance_hpp

// Test inheritance and polymorphism.

#include <iostream>

namespace ComplexLib {

    class A {
    public:
        std::string f();
    };

    class B {
    public:
        virtual std::string f();
        //virtual ~B()=0;
        virtual ~B() {}
    };

    class C : public B {};
};

#endif

