
%feature("rp:group", "conversions");
%feature("rp:include") %{
#include <cl/conversions.hpp>
%}

namespace ComplexLib {
    std::string showGrade(Grade score);
}

%feature("rp:group", "");

