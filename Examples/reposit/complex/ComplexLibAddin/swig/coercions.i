
%feature("rp:group", "coercions");
%feature("rp:include") %{
#include <cl/coercions.hpp>
%}

namespace ComplexLib {
    std::string showGrade2(Grade2 score);
}

%feature("rp:group", "");

