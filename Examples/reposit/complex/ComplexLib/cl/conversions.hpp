
#ifndef complex_lib_conversions_hpp
#define complex_lib_conversions_hpp

// Test conversions.

#include <string>

namespace ComplexLib {

    struct Grade {
        Grade(int);
        operator char();
    private:
        int score_;
    };

    std::string showGrade(Grade);
};

#endif

