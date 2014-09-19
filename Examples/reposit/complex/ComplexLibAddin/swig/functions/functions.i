
%feature("rp:group", "functions");
%feature("rp:obj_include") %{
#include <cl/functions.hpp>
%}

namespace ComplexLib {
    std::string func1();
}

%feature("rp:group", "");

