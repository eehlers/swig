
#ifndef complex_lib_coercions_hpp
#define complex_lib_coercions_hpp

// Test coercions.

#include <string>

namespace ComplexLib {

    struct Grade2 {
        Grade2(long=0);
        operator char();
    private:
        long score_;
    };

    std::string showGrade2(Grade2);
};

#endif

