
%group(typedefs);

%insert(typedefs_library_hpp) %{
#include <cl/typedefs.hpp>
%}

%insert(typedefs_addin_cpp) %{
#include <cl/typedefs.hpp>
%}

namespace ComplexLib {

    typedef double Double;
    std::string doubleToString(const Double &d);

    typedef double LongDouble;
    std::string longDoubleToString(const LongDouble &d);
}

