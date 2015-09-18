
%group(overrides);
%override

%insert(overrides_lib_inc) %{
#include <cl/overrides.hpp>
%}

namespace ComplexLib {
    class Test2 {
    public:
        Test2();
        std::string f();
    };
}

