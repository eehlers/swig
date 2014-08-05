
#ifndef complex_lib_noparm_hpp
#define complex_lib_noparm_hpp

#include <iostream>

// Test that source code is autogenerated correctly
// for constructors/members/functions that take no arguments.

namespace ComplexLib {

    void func2();

    class NoParm {
    public:
        NoParm() {
            std::cout << "NoParm::NoParm()" << std::endl;
        }
        void f() { std::cout << "NoParm::f()" << std::endl; }
    };

};

#endif
