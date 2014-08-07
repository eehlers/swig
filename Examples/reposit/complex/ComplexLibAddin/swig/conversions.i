
%feature("rp:group", "typedefs");
%feature("rp:include") %{
#include <cl/conversions.hpp>
%}

namespace ComplexLib {
    std::string showGrade(ComplexLib::Grade score);
}

