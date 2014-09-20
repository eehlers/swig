
#ifndef complex_lib_coercions_hpp
#define complex_lib_coercions_hpp

// Test coercions.

#include <string>

namespace ComplexLib {

    struct Grade2 {
        Grade2(double=0);
        operator char();
    private:
        double score_;
    };

    std::string showGrade2(Grade2);
};

#endif

