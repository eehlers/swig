
//%feature("rp:group", "typedefs");
%feature("rp:include") %{
#include <cl/typedefs.hpp>
%}

namespace ComplexLib {

    typedef double Double;
    std::string doubleToString(const Double &d);

    std::string longDoubleToString(const LongDouble &d);
}

