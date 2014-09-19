
%feature("rp:group", "typedefs");
%feature("rp:obj_include") %{
#include <cl/typedefs.hpp>
%}

namespace ComplexLib {

    typedef double Double;
    std::string doubleToString(const Double &d);

    std::string longDoubleToString(const LongDouble &d);
}

%feature("rp:group", "");

