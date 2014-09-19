
%feature("rp:group", "conversions");
%feature("rp:obj_include") %{
#include <cl/conversions.hpp>
%}

namespace ComplexLib {
    std::string showGrade(Grade score);
}

%feature("rp:group", "");

