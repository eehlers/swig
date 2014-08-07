
%feature("rp:group", "coercions");
%feature("rp:include") %{
#include <cl/coercions.hpp>
%}

namespace ComplexLib {
    std::string showGrade2(ComplexLib::Grade2 score);
}

