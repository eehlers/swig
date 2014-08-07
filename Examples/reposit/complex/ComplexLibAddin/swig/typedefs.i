
%feature("rp:group", "typedefs");
%feature("rp:include") %{
#include <cl/typedefs.hpp>
%}

namespace ComplexLib {
    std::string doubleToString(const ComplexLib::Double &d);
}

