
%group(typedefs);

%insert(typedefs_library_hpp) %{
#include <cl/typedefs.hpp>
%}

namespace ComplexLib {

    typedef double Double;
    std::string doubleToString(const Double &d);

    std::string longDoubleToString(const LongDouble &d);
}

