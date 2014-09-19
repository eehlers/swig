
%feature("rp:group", "overrides");
%feature("rp:obj_include") %{
#include <cl/overrides.hpp>
%}

%feature("rp:override_obj");

namespace ComplexLib {
    class Test2 {
    public:
        Test2();
        std::string f();
    };
}

%feature("rp:override_obj", "");

%feature("rp:group", "");

