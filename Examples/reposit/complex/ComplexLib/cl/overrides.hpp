
#ifndef complex_lib_overrides_hpp
#define complex_lib_overrides_hpp

// Test overrides.

#include <string>

namespace ComplexLib {

    class Test2 {
    public:
        std::string f() { return "ComplexLib::Test2::f()"; }
    };
};

#endif

