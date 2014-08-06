
#ifndef complex_lib_inheritance_hpp
#define complex_lib_inheritance_hpp

// Test inheritance and polymorphism.

#include <iostream>

namespace ComplexLib {

    // A) Base case - one class.

    class A {
    public:
        std::string func() { return "ComplexLib::A::func()"; }
    };

    // B) One base class, one derived.

    class BA {
    public:
        virtual std::string func()=0;
        virtual ~BA() {}
    };

    class BB : public BA {
    public:
        virtual std::string func() { return "ComplexLib::BB::func()"; }
    };

    // C) Hierarchy of 3 classes.

    class CA {
    public:
        virtual std::string func()=0;
        virtual ~CA() {}
    };

    class CB : public CA {
    public:
        virtual std::string func()=0;
    };

    class CC : public CB {
    public:
        virtual std::string func() { return "ComplexLib::CC::func()"; }
    };

};

#endif

