
#ifndef complex_lib_conversions_hpp
#define complex_lib_conversions_hpp

// Test conversions.

#include <string>

namespace ComplexLib {

    struct Grade {
        Grade(long);
        operator char();
    private:
        long score_;
    };

    std::string showGrade(Grade);
};

#endif

