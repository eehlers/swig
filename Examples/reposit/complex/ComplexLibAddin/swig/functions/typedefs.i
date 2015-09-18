
%group(typedefs);

%insert(typedefs_lib_inc) %{
#include <cl/typedefs.hpp>
%}

namespace ComplexLib {

    typedef double Double;
    std::string doubleToString(const Double &d);

    std::string longDoubleToString(const LongDouble &d);
}

