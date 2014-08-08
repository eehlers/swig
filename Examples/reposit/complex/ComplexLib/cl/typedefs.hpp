
#ifndef complex_lib_typedefs_hpp
#define complex_lib_typedefs_hpp

// Test typedefs.

#include <string>

namespace ComplexLib {

    // A typedef that is supported automatically thanks to
    // SWIG's native support of C++ typedefs.
    typedef double Double;
    std::string doubleToString(const Double &d);

    // A typedef that cannot be supported automatically because the
    // underlying type is not recognized on target platforms.
    // The addin requires a typedef to map this to rp_tp_double.
    typedef long double LongDouble;
    std::string longDoubleToString(const LongDouble &d);
};

#endif

